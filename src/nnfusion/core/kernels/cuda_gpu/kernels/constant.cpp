// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include <iostream>
#include <stdexcept>
#include <stdio.h>

#include "../cuda_emitter.hpp"
#include "../cuda_langunit.hpp"
#include "nnfusion/core/operators/generic_op/generic_op.hpp"

DECLARE_bool(fenable_cpu);

namespace nnfusion
{
    namespace kernels
    {
        namespace cuda
        {
            class Constant : public KernelEmitter
            {
            public:
                Constant(shared_ptr<KernelContext> ctx)
                    : KernelEmitter(ctx, "cuda")
                {
                    op = static_pointer_cast<nnfusion::op::Constant>(ctx->gnode->get_op_ptr());
                    NNFUSION_CHECK_NOT_NULLPTR(op) << "Node type is not Constant.";

                    folder = "./Constant/";

                    std::stringstream tag;
                    tag << "load_" << const_name;
                    custom_tag = tag.str();
                }

                LanguageUnit_p emit_function_call() override {
                    LanguageUnit_p _lu(new LanguageUnit(this->m_kernel_name + "_call"));
                    auto& lu = *_lu;
                    vector<string> names;
                    NNFUSION_CHECK(m_context->input_names.size() == 0);
                    NNFUSION_CHECK(m_context->output_names.size() == 1);
                    NNFUSION_CHECK(m_context->tensor_names.size() == 0);
                    names.push_back(m_context->output_names[0]);
                    if (FLAGS_fenable_cpu) names.push_back(m_context->output_names[0] + "_cpu");
                    lu << "(";

                    auto gnode = m_context->gnode;
                    if (m_function_unit != nullptr)
                    {
                        auto sig_unit = m_function_unit->signature_unit;
                        NNFUSION_CHECK_NOT_NULLPTR(sig_unit);
                        if (gnode && (*gnode)["Async_info"].is_valid())
                        {
                            auto& async_info = (*gnode)["Async_info"].as<nnfusion::async::AsyncExecutionInfo>();
                            auto stream = async_info.execution_stream;
                            if (stream)
                            {
                                if (sig_unit->get_code().find("cudaStream_t") != string::npos)
                                    lu << stream->get_name() << ", ";
                            }
                        }
                    }
                    lu << join(names, ", ") << ");\n";
                    return _lu;
                }

                LanguageUnit_p emit_function_body() override
                {
                    nnfusion::codegen::create_folder(folder);
                    const_name = m_context->outputs[0]->get_name();
                    ofstream bin_file(folder + const_name + ".bin", ios::out | ios::binary);
                    bin_file.write((const char*)op->get_data_ptr(), op->get_data_size());
                    bin_file.close();

                    // NNFUSION_LOG(INFO) << "Emitting Constant [" << const_name << "], {"
                    //                    << op->get_unique_name() << "}, (" << op->get_data_size()
                    //                    << "), " << op->get_type();

                    LanguageUnit_p _lu(new LanguageUnit(get_function_name()));
                    auto& writer = *_lu;
                    if (FLAGS_fenable_cpu) {
                        writer << "std::ifstream bin_file(\"" << folder + const_name
                            << ".bin\" , std::ios::in | std::ios::binary);\n"
                            << "if(bin_file.fail())\n"
                            << "{\n"
                            << "\tprintf(\"Load " << const_name << " failed.\\n\");\n"
                            << "\texit(1);\n"
                            << "}\n"
                            << "bin_file.read((char*)output0_cpu, " << op->get_data_size() << ");\n"
                            << "cudaMemcpyAsync(output0, output0_cpu, " << op->get_data_size()
                            << ", cudaMemcpyHostToDevice, stream);\n"
                            << "bin_file.close();\n";
                    } else {
                        writer << "std::ifstream bin_file(\"" << folder + const_name
                            << ".bin\" , std::ios::in | std::ios::binary);\n"
                            << "if(bin_file.fail())\n"
                            << "{\n"
                            << "\tprintf(\"Load " << const_name << " failed.\\n\");\n"
                            << "\texit(1);\n"
                            << "}\n"
                            << "char* tmp_mem = new char[" << op->get_data_size() << "];\n"
                            << "bin_file.read(tmp_mem, " << op->get_data_size() << ");\n"
                            << "cudaMemcpyAsync(output0, tmp_mem, " << op->get_data_size()
                            << ", cudaMemcpyHostToDevice, stream);\n"
                            << "bin_file.close();\n";
                    }
                    return _lu;
                }

                LanguageUnit_p emit_dependency() override

                {
                    LanguageUnit_p _lu(new LanguageUnit(get_function_name() + "_dep"));
                    _lu->require(header::cuda);
                    _lu->require(header::fstream);
                    return _lu;
                }

                LanguageUnit_p emit_function_signature() override
                {
                    LanguageUnit_p _lu(new LanguageUnit(this->m_kernel_name + "_sig"));
                    auto& lu = *_lu;

                    vector<string> params;
                    for (size_t i = 0; i < m_context->inputs.size(); i++)
                    {
                        stringstream ss;
                        ss << m_context->inputs[i]->get_element_type().c_type_string() << "* ";
                        ss << "input" << i;
                        params.push_back(ss.str());
                    }

                    for (size_t i = 0; i < m_context->outputs.size(); i++)
                    {
                        stringstream ss;
                        ss << m_context->outputs[i]->get_element_type().c_type_string() << "* ";
                        ss << "output" << i;
                        params.push_back(ss.str());
                    }

                    for (size_t i = 0; i < m_context->tensors.size(); i++)
                    {
                        stringstream ss;
                        ss << m_context->tensors[i]->get_element_type().c_type_string() << "* ";
                        // defult name is: "persit0", "persist1" ...
                        ss << m_context->tensors[i]->get_name();
                        params.push_back(ss.str());
                    }

                    if (FLAGS_fenable_cpu) {
                        for (size_t i = 0; i < m_context->inputs.size(); i++)
                        {
                            stringstream ss;
                            ss << m_context->inputs[i]->get_element_type().c_type_string() << "* ";
                            ss << "input" << i << "_cpu";
                            params.push_back(ss.str());
                        }

                        for (size_t i = 0; i < m_context->outputs.size(); i++)
                        {
                            stringstream ss;
                            ss << m_context->outputs[i]->get_element_type().c_type_string() << "* ";
                            ss << "output" << i << "_cpu";
                            params.push_back(ss.str());
                        }

                        for (size_t i = 0; i < m_context->tensors.size(); i++)
                        {
                            stringstream ss;
                            ss << m_context->tensors[i]->get_element_type().c_type_string() << "* ";
                            // defult name is: "persit0", "persist1" ...
                            ss << m_context->tensors[i]->get_name() << "_cpu";
                            params.push_back(ss.str());
                        }
                    }

                    lu << "void "
                       << "(cudaStream_t stream, " << join(params, ", ") << ")";
                    return _lu;
                }

            private:
                shared_ptr<KernelContext> kernel_ctx;
                shared_ptr<nnfusion::op::Constant> op;
                string folder;
                string const_name;
            };
        } // namespace cuda
    }     // namespace kernels
} // namespace nnfusion

// Register Pad kernel emitter

using namespace nnfusion;
using namespace nnfusion::kernels;
REGISTER_KERNEL_EMITTER("Constant",                                                //op_name
                        Device(CUDA_GPU).TypeConstraint(element::f32).Priority(2), //attrs
                        cuda::Constant)                                            // constructor