// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once
#include "../../cpu_op_emitter.hpp"
#include "../../cuda_langunit.hpp"

namespace nnfusion
{
    namespace kernels
    {
        namespace cuda_cpu
        {
            class Slice : public CPUOpEmitter
            {
            public:
                Slice(shared_ptr<KernelContext> ctx);

                LanguageUnit_p emit_function_body() override;
                LanguageUnit_p emit_dependency() override;
                bool is_eliminative() override;

            private:
                nnfusion::Shape input_shape, output_shape, lower_bounds;
                nnfusion::Shape input_strides, output_strides, slice_strides;
                string input_type, output_type;
                bool is_memcpy = false;
                size_t input_offset;
                size_t data_type_size;
            };

        } // namespace cuda
    }     // namespace kernels
} // namespace nnfusion