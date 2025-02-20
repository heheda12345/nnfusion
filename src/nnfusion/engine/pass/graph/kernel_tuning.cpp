// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "kernel_tuning.hpp"
#include <queue>
#include <sstream>
#include <utility>

#include "nnfusion/core/kernels/cpu/cpu_kernel_emitter.hpp"
#include "nnfusion/core/kernels/cuda_gpu/cuda_emitter.hpp"
#include "nnfusion/core/kernels/hlsl/hlsl_kernel_emitter.hpp"
#include "nnfusion/util/curl_request.hpp"

using namespace nnfusion;
using namespace nnfusion::graph;
using namespace nnfusion::kernels;
using namespace nnfusion::pass::graph;

DEFINE_int64(fkernel_tuning_steps, 0, "Enable automatic kernel tuning for maximum N steps.");
DEFINE_string(ftuning_blocklist,
              "",
              "List of op types that skip kernel tuning pass, e.g., \"Softmax,Add\"");
DEFINE_string(ftuning_list, "", "List of op types for kernel tuning pass, e.g., \"Softmax,Add\"");
DEFINE_string(fantares_perf_file, "./antares_perf.csv", "File to save Antares kernel performance.");
DECLARE_bool(fantares_mode);
DECLARE_string(fantares_codegen_server);
DECLARE_string(fproduct_name);
DECLARE_string(fdefault_device);

struct TuningStatus
{
    TuningStatus(std::shared_ptr<GNode> gnode)
        : op_type(gnode->get_op_type())
        , op_name(gnode->get_op_ptr()->get_name())
        , progress_step(0)
        , best_perf(-1.0)
    {
    }
    std::string op_type;
    std::string op_name;
    std::string status;
    int64_t progress_step;
    double best_perf;
    std::string ir;
};

std::string send_tuning_request(std::string& ir, int64_t step)
{
    CurlRequest req(FLAGS_fantares_codegen_server);
    req.add_custom_header(("COMPUTE_V1: " + ir).c_str());
    req.add_custom_header(("STEP: " + std::to_string(step)).c_str());

    std::string response;
    if (!req.send_request(response))
    {
        NNFUSION_LOG(ERROR) << "Error request for IR: " << ir;
    }
    // NNFUSION_LOG(INFO) << response;
    if (strncmp(response.c_str(), "[ERROR]", 7) == 0)
    {
        NNFUSION_LOG(ERROR) << ir << "\n" << response;
    }

    return response;
}

void print_tuning_results(std::vector<std::shared_ptr<TuningStatus>> tuned_kernels,
                          std::vector<std::shared_ptr<TuningStatus>> tuning_kernels)
{
    std::stringstream ss;
    ss << " Kernel Tuning Status: \n";
    ss << " NOTE: the tuning progress (N/M) means that the current best kernel is searched at the "
          "N-th step of the total M steps. \n\n";
    ss << " | " << std::setw(20) << "OP"
       << " | " << std::setw(26) << "NAME"
       << " | " << std::setw(10) << "STATUS"
       << " | " << std::setw(10) << "PROGRESS"
       << " | " << std::setw(18) << "PERFORMANCE |\n";

    ss << " | " << std::setfill('-') << setw(96) << " |\n";
    ss << std::setfill(' ');

    for (auto s : tuned_kernels)
    {
        ss << " | " << std::setw(20) << s->op_type << " | " << std::setw(26)
           << ((s->op_name.size() > 26) ? (s->op_name.substr(0, 24) + "..") : s->op_name) << " | "
           << std::setw(10) << s->status << " | " << std::setw(6) << s->progress_step << "/"
           << FLAGS_fkernel_tuning_steps << " "
           << " | " << std::setw(12) << s->best_perf << " ms |\n";
    }
    for (auto s : tuning_kernels)
    {
        ss << " | " << std::setw(20) << s->op_type << " | " << std::setw(26)
           << ((s->op_name.size() > 26) ? (s->op_name.substr(0, 24) + "..") : s->op_name) << " | "
           << std::setw(10) << s->status << " | " << std::setw(6) << s->progress_step << "/"
           << FLAGS_fkernel_tuning_steps << " "
           << " | " << std::setw(12) << s->best_perf << " ms |\n";
    }
    NNFUSION_LOG(INFO) << ss.str();
}

void dump_perf(std::string filename,
               std::vector<std::shared_ptr<TuningStatus>> tuned_kernels,
               std::unordered_map<std::string, size_t> ir_cnt)
{
    double total_time = 0.0;
    for (auto status : tuned_kernels)
    {
        if (status->best_perf > 0)
            total_time += status->best_perf * ir_cnt.at(status->ir);
    }
    std::sort(tuned_kernels.begin(),
              tuned_kernels.end(),
              [&](std::shared_ptr<TuningStatus> left, std::shared_ptr<TuningStatus> right) {
                  return left->best_perf * ir_cnt.at(left->ir) >
                         right->best_perf * ir_cnt.at(right->ir);
              });
    std::ofstream out(FLAGS_fantares_perf_file);
    for (auto status : tuned_kernels)
    {
        size_t cnt = ir_cnt.at(status->ir);
        double kernel_time_sum = status->best_perf > 0 ? status->best_perf * cnt : -1.0;
        double percent = std::max(kernel_time_sum / total_time * 100, 0.0);

        out << std::fixed << std::setprecision(2) << percent << "%"
            << "\t" << kernel_time_sum << "\t" << cnt << "\t" << status->best_perf << "\t"
            << status->op_type << "\t" << status->op_name << "\t" << status->ir << endl;
    }
    out.close();
}

std::pair<std::vector<std::shared_ptr<GNode>>, std::vector<std::shared_ptr<TuningStatus>>>
    get_tuning_candidates(std::shared_ptr<nnfusion::graph::Graph>& graph,
                          const std::unordered_set<std::string> tuning_list,
                          const std::unordered_set<std::string> block_list,
                          std::unordered_map<std::string, size_t>& ir2cnt)
{
    NNFUSION_CHECK(graph != nullptr);

    std::vector<std::shared_ptr<GNode>> candidates;
    std::vector<std::shared_ptr<GNode>> nodes = graph->get_nodes();
    for (auto gnode : nodes)
    {
        if (!(*gnode)["DeviceType"].is_valid())
        {
            NNFUSION_CHECK_FAIL() << "GNode DeviceType should be assigned before this pass："
                                  << gnode->get_name();
        }
        auto n_device_type = (*gnode)["DeviceType"].as<NNFusion_DeviceType>();
        NNFUSION_CHECK(n_device_type != UNKNOWN);

        // filter ops not in TuningList
        if (tuning_list.size() > 0 && tuning_list.find(gnode->get_op_type()) == tuning_list.end())
        {
            continue;
        }

        // filter ops in BlockList
        if (block_list.find(gnode->get_op_type()) != block_list.end())
        {
            continue;
        }

        auto ir = nnfusion::op::get_translation(gnode);
        // NNFUSION_LOG(DEBUG) << gnode->get_op_type() << ", ir: " << ir;

        // filter unimplemented irs
        if (ir.empty())
        {
            continue;
        }

        // dedupe ops with the same ir
        if (ir2cnt.find(ir) != ir2cnt.end())
        {
            ir2cnt.at(ir) += 1;
        }
        else
        {
            ir2cnt[ir] = 1;
            candidates.push_back(gnode);
        }
    }

    // filter ops existing in kernel cache DB
    std::vector<std::shared_ptr<TuningStatus>> cached_kernels;
    {
        auto cache_manager = std::make_shared<cache::KernelCacheManager>();
        if (!cache_manager->is_valid())
        {
            NNFUSION_LOG(INFO) << "No valid kernel cache, all the kernels will be tuned";
        }
        else
        {
            std::unordered_map<std::string, std::shared_ptr<TuningStatus>> ir2kernel;
            std::vector<std::shared_ptr<GNode>> non_cached_candidates;
            for (auto gnode : candidates)
            {
                auto ir = nnfusion::op::get_translation(gnode);
                shared_ptr<KernelContext> ctx(new KernelContext(gnode));
                auto identifier = ctx->generate_identifier();
                auto device_type = get_device_str((*gnode)["DeviceType"].as<NNFusion_DeviceType>());
                std::string source = "Antares";
                auto fetched = cache_manager->fetch_with_source(identifier, device_type, source);
                if (device_type != "CUDA_GPU")
                {
                    NNFUSION_CHECK(fetched.size() == 0);
                }

                bool tune_flag = true;
                for (auto fetch : fetched)
                {
                    if (fetch->miscs["antares"]["device_name"] == FLAGS_fproduct_name &&
                        fetch->miscs["antares"]["planned_steps"] >= FLAGS_fkernel_tuning_steps)
                    {
                        double fetch_perf = double(fetch->miscs["antares"]["time"]) / 1000;
                        // ignore kernel without perf
                        if (fetch_perf <= 0)
                        {
                            continue;
                        }
                        // ignore current kernel if we have a better kernel
                        if (ir2kernel.find(ir) != ir2kernel.end() &&
                            ir2kernel.at(ir)->best_perf <= fetch_perf)
                        {
                            continue;
                        }
                        auto status = std::make_shared<TuningStatus>(gnode);
                        status->status = "completed";
                        status->progress_step = fetch->miscs["antares"]["step_produced"];
                        status->best_perf = fetch_perf;
                        status->ir = ir;
                        ir2kernel[ir] = status;
                        tune_flag = false;
                    }
                }
                if (tune_flag)
                {
                    non_cached_candidates.push_back(gnode);
                }
                else
                {
                    cached_kernels.push_back(ir2kernel.at(ir));
                }
            }
            candidates = non_cached_candidates;
        }
    }

    return std::make_pair(candidates, cached_kernels);
}

bool KernelTuning::parse_block_list()
{
    auto blocklist_str = FLAGS_ftuning_blocklist;
    stringstream ss(blocklist_str);
    while (ss.good())
    {
        string substr;
        getline(ss, substr, ',');
        BlockList.insert(substr);
    }
    NNFUSION_LOG(INFO) << "Kernel Tuning BlockList: " << join(BlockList, ", ");
    return true;
}

bool KernelTuning::parse_tuning_list()
{
    auto tuninglist_str = FLAGS_ftuning_list;
    stringstream ss(tuninglist_str);
    while (ss.good())
    {
        string substr;
        getline(ss, substr, ',');
        TuningList.insert(substr);
    }
    NNFUSION_LOG(INFO) << "Kernel Tuning List: " << join(TuningList, ", ");
    return true;
}

bool KernelTuning::run_on_graph(std::shared_ptr<nnfusion::graph::Graph>& graph)
{
    if (FLAGS_fantares_mode)
    {
        parse_tuning_list();
        parse_block_list();
        for (auto item : TuningList)
        {
            NNFUSION_CHECK(BlockList.find(item) == BlockList.end())
                << "Kernel Tuning Pass: There are same operators in TuningList and "
                   "TuningBlockList.";
        }
        // register antares kernels anyway here in case kernel selection pass will use them
        register_antares_kernel();
    }

    if (FLAGS_fkernel_tuning_steps <= 0 || FLAGS_fantares_codegen_server == "" ||
        !FLAGS_fantares_mode)
    {
        return true;
    }

    std::vector<std::shared_ptr<TuningStatus>> tuned_kernels;
    std::vector<std::shared_ptr<TuningStatus>> tuning_kernels;
    std::unordered_map<std::string, size_t> ir2cnt;
    std::vector<std::shared_ptr<GNode>> nodes;
    std::tie(nodes, tuned_kernels) = get_tuning_candidates(graph, TuningList, BlockList, ir2cnt);
    for (auto gnode : nodes)
    {
        if (!(*gnode)["DeviceType"].is_valid())
        {
            NNFUSION_CHECK_FAIL() << "GNode DeviceType should be assigned before this pass："
                                  << gnode->get_name();
        }
        auto n_device_type = (*gnode)["DeviceType"].as<NNFusion_DeviceType>();
        NNFUSION_CHECK(n_device_type != UNKNOWN);

        auto ir = nnfusion::op::get_translation(gnode);
        // NNFUSION_LOG(INFO) << gnode->get_op_type() << ", ir: " << ir;
        if (!ir.empty())
        {
            auto status = std::make_shared<TuningStatus>(gnode);
            status->ir = ir;
            auto response = send_tuning_request(ir, 0);

            auto start = response.find("\n// Saved Perf =");
            if (start != std::string::npos)
            {
                auto tail_info = response.substr(start, string::npos);
                std::regex ws_re("\\s+");
                std::vector<std::string> items(
                    std::sregex_token_iterator(tail_info.begin(), tail_info.end(), ws_re, -1),
                    std::sregex_token_iterator());
                NNFUSION_CHECK(items.size() >= 16);

                double perf = std::stod(items[5]);
                int64_t best_step = std::stol(items[12]);
                int64_t plan_step = std::stol(items[16]);
                // NNFUSION_LOG(INFO) << "best perf: " << perf << "s, step: " << best_step;
                status->progress_step = best_step;
                status->best_perf = (perf < 0) ? -1 : perf * 1000.0;

                if (plan_step >= FLAGS_fkernel_tuning_steps)
                {
                    // no need to submit new tuning job
                    auto compelete_flag = response.find("Antares Tuning Completed in ");
                    status->status = (compelete_flag == string::npos) ? "tuning" : "completed";
                }
            }

            if (status->status == "" || status->status.empty())
            {
                // submit a new tuning task
                NNFUSION_LOG(INFO) << gnode->get_op_type() << ", ir: " << ir;
                auto response = send_tuning_request(ir, FLAGS_fkernel_tuning_steps);
                status->status = "submitted";
            }

            status->status == "completed" ? tuned_kernels.push_back(status)
                                          : tuning_kernels.push_back(status);
        }
    }
    print_tuning_results(tuned_kernels, tuning_kernels);

    if (tuning_kernels.size() > 0)
    {
        NNFUSION_LOG(NNFUSION_WARNING)
            << "There are pending tuning kernels. Please retry the compilation later!";
        exit(0);
    }

    dump_perf(FLAGS_fantares_perf_file, tuned_kernels, ir2cnt);
    if (FLAGS_fdefault_device == "CUDA")
    {
        // insert_to_kernel_cache(nodes);
    }

    return true;
}

void KernelTuning::register_single_kernel(const std::string& op_name)
{
    kernels::KernelRegistrar kernel_registrar_cuda(
        op_name,
        kernels::Name(op_name)
            .Device(CUDA_GPU)
            .TypeConstraint(element::f32)
            .Tag("antares")
            .Priority(9)
            .KernelFactory([](shared_ptr<kernels::KernelContext> context)
                               -> shared_ptr<kernels::KernelEmitter> {
                                   return make_shared<kernels::cuda::AntaresCudaKernelEmitter>(
                                       context);
                               })
            .Build());
    kernels::KernelRegistrar kernel_registrar_cpu(
        op_name,
        kernels::Name(op_name)
            .Device(GENERIC_CPU)
            .TypeConstraint(element::f32)
            .Tag("antares")
            .Priority(9)
            .KernelFactory([](shared_ptr<kernels::KernelContext> context)
                               -> shared_ptr<kernels::KernelEmitter> {
                                   return make_shared<kernels::cpu::AntaresCpuKernelEmitter>(
                                       context);
                               })
            .Build());
    kernels::KernelRegistrar kernel_registrar_hlsl(
        op_name,
        kernels::Name(op_name)
            .Device(HLSL)
            .TypeConstraint(element::f32)
            .Tag("antares")
            .Priority(9)
            .KernelFactory([](shared_ptr<kernels::KernelContext> context)
                               -> shared_ptr<kernels::KernelEmitter> {
                                   return make_shared<kernels::hlsl::AntaresHLSLKernelEmitter>(
                                       context);
                               })
            .Build());
}

bool KernelTuning::register_antares_kernel()
{
    for (auto pair : nnfusion::op::get_op_configs())
    {
        std::string op_name = pair.first;
        std::vector<NNFusion_DeviceType> devs{CUDA_GPU, GENERIC_CPU, HLSL};

        // skip op in BlockList
        if (BlockList.find(op_name) != BlockList.end())
        {
            continue;
        }
        register_single_kernel(op_name);
    }
    return true;
}

bool KernelTuning::insert_to_kernel_cache(const std::vector<std::shared_ptr<GNode>>& nodes)
{
    auto cache_manager = std::make_shared<cache::KernelCacheManager>();
    if (!cache_manager->is_valid())
    {
        NNFUSION_LOG(INFO)
            << "No valid kernel cache, tuned kernels will not be inserted to kernel cache DB";
        return true;
    }

    for (auto gnode : nodes)
    {
        shared_ptr<KernelContext> ctx(new KernelContext(gnode));
        std::vector<shared_ptr<const KernelRegistration>> kernel_regs =
            KernelRegistry::Global()->FindKernelRegistrations(
                gnode->get_op_type(), CUDA_GPU, element::f32);

        for (auto kernel_reg : kernel_regs)
        {
            if (kernel_reg->m_tag == "antares")
            {
                auto antares_kernel = kernel_reg->m_factory(ctx);
                auto kernel_cache_entry = antares_kernel->get_kernel_cache_entry();
                if (kernel_cache_entry == nullptr)
                {
                    NNFUSION_LOG(INFO)
                        << "Invalid kernel_cache_entry, will not insert to kernel cache: "
                        << gnode->get_name();
                }

                // overwrite existing kernel entries
                cache_manager->insert_kernel_entry(kernel_cache_entry, true);
            }
        }
    }
    return true;
}
