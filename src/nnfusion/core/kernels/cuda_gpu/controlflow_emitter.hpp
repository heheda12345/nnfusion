#pragma once
#include "cuda_emitter.hpp"
#include "cuda_langunit.hpp"
#include "nnfusion/engine/interpreter.hpp"

namespace nnfusion
{
    namespace kernels
    {
        namespace cuda
        {
            static inline dim3 maxdim3(dim3 lhs, dim3 rhs)
            {
                return dim3(std::max(lhs.x, rhs.x), std::max(lhs.y, rhs.y), std::max(lhs.z, rhs.z));
            }
            class ControlFlowEmitter : public CudaEmitter
            {
            public:
                using CudaEmitter::CudaEmitter;
                LanguageUnit_p emit_device_function_body() override;
                LanguageUnit_p emit_function_call() override;
                LanguageUnit_p emit_function_call(std::vector<std::string> names) override;
                LanguageUnit_p emit_function_signature() override;
                LanguageUnit_p emit_block_kernel_call(std::vector<std::string> params) override;
                LanguageUnit_p emit_device_function_signature() override;
                LanguageUnit_p emit_block_executor_instruction_step_to(int max_block);
                LanguageUnit_p emit_block_executor_instruction_wait_for(int max_block_last, int max_block_cur=-1);
                virtual bool is_host_kernel_launch() = 0;

            protected:
                static std::pair<cuda::dim3, cuda::dim3>
                    get_subgraph_launch_config(ir::BasicBlock::Pointer instructions);

                static std::map<std::string, int> get_subgraph_inputs(const ir::Program& program);

                void allocate_shared_memory(LanguageUnit_p _lu);
                void allocate_shared_memory(LanguageUnit_p _lu, size_t shared_memory_size);

                virtual ir::BasicBlock::Pointer create_param_map(
                    const ir::Program& program,
                    const std::unordered_map<std::string, int>& subgraph_output_map,
                    bool call_on_cuda);

                std::string get_workspace_tensor(nnfusion::descriptor::Tensor::Pointer tensor);
                std::string get_launch_bound(nnfusion::ir::Instruction::Pointer ins);
                size_t get_kernel_shared_memory(std::shared_ptr<KernelEmitter> kernel);
                size_t get_subgraph_shared_memory(const ir::Program& program);
                size_t get_inst_max_shared_memory(nnfusion::ir::BasicBlock::Pointer bb, int start_id = 0, int end_id = -1);
                size_t m_shared_memory_size = 0;
                bool is_emitting_block_kernel = false;
                descriptor::Tensor::Pointer m_workspace;
                std::unordered_map<nnfusion::descriptor::Tensor::Pointer, std::string> m_param_map;
                std::unordered_map<std::string, size_t> m_pool_offset;
                descriptor::Tensor::Pointer m_sync_tensor;
            };
        }
    }
}
