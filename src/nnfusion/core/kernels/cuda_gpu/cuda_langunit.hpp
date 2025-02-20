// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once
#include "nnfusion/core/kernels/common_langunit.hpp"

namespace nnfusion
{
    namespace kernels
    {
        namespace header
        {
            LU_DECLARE(cuda);
            LU_DECLARE(cublas);
            LU_DECLARE(cudnn);
            LU_DECLARE(superscaler);
            LU_DECLARE(cupti);
            LU_DECLARE(cuda_prof_api);
            LU_DECLARE(cuda_fp16);
            LU_DECLARE(cub);
            LU_DECLARE(math_constants);
        } // namespace header

        namespace macro
        {
            LU_DECLARE(HALF_MAX);
            LU_DECLARE(CUDA_SAFE_CALL_NO_THROW);
            LU_DECLARE(CUDA_SAFE_CALL);
            LU_DECLARE(CUDNN_SAFE_CALL_NO_THROW);
            LU_DECLARE(CUDNN_SAFE_CALL);
            LU_DECLARE(CUBLAS_SAFE_CALL_NO_THROW);
            LU_DECLARE(CUBLAS_SAFE_CALL);
            LU_DECLARE(CUDA_SAFE_LAUNCH);
            LU_DECLARE(CUPTI_CALL);
            LU_DECLARE(DBG_TENSOR);
        } // namespace macro

        namespace declaration
        {
            LU_DECLARE(division_by_invariant_multiplication);
            LU_DECLARE(rocm_division_by_invariant_multiplication);
            LU_DECLARE(cuda_cpu_division_by_invariant_multiplication);
            LU_DECLARE(load);
            LU_DECLARE(mad16);
            LU_DECLARE(mod16);
            LU_DECLARE(global_cublas_handle);
            LU_DECLARE(global_cudnn_handle);
            LU_DECLARE(num_SMs);
            LU_DECLARE(cuda_reduce_primitive);
            LU_DECLARE(cuda_layer_norm);
            LU_DECLARE(cuda_fp16_scale);
            LU_DECLARE(ort_layer_norm);
            LU_DECLARE(ort_qkv_to_context);
            LU_DECLARE(cuda_convert_template);
            LU_DECLARE(math_Rsqrt);
            LU_DECLARE(math_Gelu);
            LU_DECLARE(ort_softmax);
            LU_DECLARE(warp);
            LU_DECLARE(barrier);
            LU_DECLARE(manual_barrier);
            LU_DECLARE(inc_iter);
            LU_DECLARE(step_to_device);
            LU_DECLARE(block_barrier);
        } // namespace declaration
    }     // namespace kernels
} // namespace nnfusion
