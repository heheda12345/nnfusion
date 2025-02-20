# Copyright (c) Microsoft Corporation.
# Licensed under the MIT License.


set(SRC
    cuda_emitter.cpp
    cuda_cudnn.cpp
    cuda_helper.cpp
    cuda_langunit.cpp
    controlflow_emitter.cpp
    cpu_op_emitter.cpp
    cuda_testutil.cpp
    util/gpu_util.cpp
)

file(GLOB wild_cuda_kernels kernels/*.cpp)
file(GLOB wild_rocm_kernels kernels/rocm/*.cpp)
file(GLOB wild_cuda_cpu_kernels kernels/cpu_op/*.cpp)

set(KERNELS
    ${wild_cuda_kernels}
    ${wild_rocm_kernels}
    ${wild_cuda_cpu_kernels}
)

add_library(kernels_cuda STATIC ${SRC} ${KERNELS})
target_include_directories(kernels_cuda SYSTEM PUBLIC
    ${GLOBAL_INCLUDE_PATH}
)
