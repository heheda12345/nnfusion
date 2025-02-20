// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include "../cuda_emitter.hpp"
#include "../cuda_langunit.hpp"

namespace nnfusion
{
    namespace kernels
    {
        namespace cuda
        {
            class ScatterND : public BlockCudaEmitter
            {
            public:
                ScatterND(shared_ptr<KernelContext> ctx);

                LanguageUnit_p emit_function_body() override;
                LanguageUnit_p emit_dependency() override;
                // LanguageUnit_p emit_dependency() override;
                // LanguageUnit_p emit_function_name() override;
                // LanguageUnit_p emit_comments() override;

            protected:
                void set_launch_config() override;

            private:
                uint32_t update_shape;
            };

        } // namespace cuda
    }     // namespace kernels
} // namespace nnfusion