// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "../../cpu_op_emitter.hpp"
#include "../../cuda_langunit.hpp"

namespace nnfusion
{
    namespace kernels
    {
        namespace cuda_cpu
        {
            class Broadcast : public CPUOpEmitter
            {
            public:
                Broadcast(shared_ptr<KernelContext> ctx)
                    : CPUOpEmitter(ctx)
                {
                    auto _op =
                        static_pointer_cast<nnfusion::op::Broadcast>(ctx->gnode->get_op_ptr());
                    NNFUSION_CHECK_NOT_NULLPTR(_op) << "Node type is not Broadcast.";
                    auto& axes = _op->get_broadcast_axes();
                    if (!axes.empty())
                    {
                        this->axes = AxisSet(axes);
                    }

                    result_shape = ctx->outputs[0]->get_shape();

                    // calculate strides
                    strides = nnfusion::row_major_strides(result_shape);
                    // precacluate invariants for integer division via multiplication
                    stride_magic;
                    stride_shift;
                    for (int i = 0; i < strides.size(); i++)
                    {
                        int magic;
                        int shift;
                        std::tie(magic, shift) = cuda::idiv_magic_u64(strides[i]);
                        stride_magic.push_back(magic);
                        stride_shift.push_back(shift);
                    }
                    // calculate reduced tensor strides with 0s inserted for reduced axes
                    reduced_shape = result_shape;
                    for (auto const& axis : axes)
                    {
                        reduced_shape[axis] = 1;
                    }
                    reduced_strides = nnfusion::row_major_strides(reduced_shape);
                    for (auto const& axis : axes)
                    {
                        reduced_strides[axis] = 0;
                    }

                    rank = result_shape.size();

                    std::stringstream tag;
                    tag << "_s" << join(result_shape, "_") << "_rs" << join(this->axes, "_");
                    custom_tag = tag.str();

                    // add inplace tag
                    is_memcpy = false;
                    auto arg_shape = ctx->inputs[0]->get_shape();
                    if (axes.empty() || shape_size(arg_shape) == shape_size(result_shape))
                    {
                        is_memcpy = true;
                        if (!ctx->annotations)
                        {
                            ctx->annotations = std::make_shared<Annotations>();
                        }
                        ctx->annotations->add_in_place_oi_pair({0, 0, false});
                    }
                }

                bool is_eliminative() override
                {
                    if (is_memcpy && m_context->inputs[0]->is_same_address(m_context->outputs[0]))
                        return true;
                    else
                        return false;
                }

                LanguageUnit_p emit_function_body() override
                {
                    LanguageUnit_p _lu(new LanguageUnit(get_function_name()));

                    // function signature:
                    // extern "C" __global__ void kernel(m_context->dtypes[0]* input0, m_context->dtypes[0]* input1, m_context->dtypes[2]* output0)
                    auto& writer = *_lu;

                    auto expand_vector_int = [](string name, vector<int>& d) {
                        stringstream ss;
                        for (int i = 0; i < d.size(); i++)
                            ss << "int " << name << i << " = " << to_string(d[i]) << ";\n";
                        return ss.str();
                    };

                    auto expand_vector_uint32 = [](string name, vector<uint32_t>& d) {
                        stringstream ss;
                        for (int i = 0; i < d.size(); i++)
                            ss << "uint32_t " << name << i << " = " << to_string(d[i]) << ";\n";
                        return ss.str();
                    };

                    writer << "size_t nthreads = " << shape_size(result_shape) << ";\n";

                    writer << expand_vector_uint32("strides", strides)
                           << expand_vector_int("stride_magic", stride_magic)
                           << expand_vector_int("stride_shift", stride_shift)
                           << expand_vector_uint32("reduced_strides", reduced_strides);
                        //    << "const int tid = blockDim.x * blockIdx.x + threadIdx.x;\n";
                    writer << "for (int tid = 0; tid < nthreads; tid++)\n";
                    writer.block_begin();
                    {
                        // calculate tensor coordinates (inverse tensor reduction)
                        std::string reduced_idx =
                            cuda::collective_coordinate_transform_helper(writer,
                                                                   "tid",
                                                                   "strides",
                                                                   "stride_magic",
                                                                   "stride_shift",
                                                                   "reduced_strides",
                                                                   "coordinate",
                                                                   rank,
                                                                   true,
                                                                   "reduced_idx",
                                                                   "division_by_invariant_multiplication_cpu");
                        writer << "output0[tid] = input0[" << reduced_idx << "];\n";
                    }
                    writer.block_end();
                    return _lu;
                }

                virtual LanguageUnit_p get_division_by_invariant_multiplication()
                {
                    return declaration::cuda_cpu_division_by_invariant_multiplication;
                }

                LanguageUnit_p emit_dependency() override

                {
                    LanguageUnit_p _lu(new LanguageUnit(get_function_name() + "_dep"));
                    _lu->require(header::cuda);
                    _lu->require(get_division_by_invariant_multiplication());
                    _lu->require(declaration::load);
                    return _lu;
                }

            private:
                shared_ptr<KernelContext> kernel_ctx;
                shared_ptr<nnfusion::op::Pad> _op;

                // calculate strides
                nnfusion::NVShape strides;
                // precacluate invariants for integer division via multiplication
                std::vector<int> stride_magic;
                std::vector<int> stride_shift;
                // calculate reduced tensor strides with 0s inserted for reduced axes
                nnfusion::NVShape reduced_shape;
                nnfusion::NVShape reduced_strides;
                nnfusion::Shape result_shape;
                size_t rank;
                AxisSet axes;
                bool is_memcpy = false;
            };

        } // namespace cuda
    }     // namespace kernels
} // namespace nnfusion

// Register Pad kernel emitter

using namespace nnfusion;
using namespace nnfusion::kernels;
REGISTER_KERNEL_EMITTER("Broadcast",                                               //op_name
                        Device(SINGLE_CPU).TypeConstraint(element::f32).Priority(2), //attrs
                        cuda_cpu::Broadcast)                                           // constructor
