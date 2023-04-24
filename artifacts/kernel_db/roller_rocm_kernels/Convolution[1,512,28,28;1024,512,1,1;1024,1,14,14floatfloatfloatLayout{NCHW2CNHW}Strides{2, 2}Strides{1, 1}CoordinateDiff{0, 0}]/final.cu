
#ifdef _WIN32
  using uint = unsigned int;
  using uchar = unsigned char;
  using ushort = unsigned short;
  using int64_t = long long;
  using uint64_t = unsigned long long;
#else
  #define uint unsigned int
  #define uchar unsigned char
  #define ushort unsigned short
  #define int64_t long long
  #define uint64_t unsigned long long
#endif
extern "C" __global__ void __launch_bounds__(256) default_function_kernel0(float* __restrict__ data, float* __restrict__ kernel, float* __restrict__ conv_unpad) {
  float conv_local[2];
  __shared__ float data_pad_shared[256];
  __shared__ float kernel_pad_shared[512];
  float data_pad_shared_local[1];
  float kernel_pad_shared_local[2];
  conv_local[0] = 0.000000e+00f;
  conv_local[1] = 0.000000e+00f;
  for (int ra_fused0_outer = 0; ra_fused0_outer < 32; ++ra_fused0_outer) {
    __syncthreads();
    data_pad_shared[((int)threadIdx.x)] = (((((((int)blockIdx.x) % 13) * 4) + ((((int)threadIdx.x) & 15) >> 2)) < 49) ? data[((((ra_fused0_outer * 12544) + ((((int)threadIdx.x) >> 4) * 784)) + (((((((int)blockIdx.x) % 13) * 8) + ((((int)threadIdx.x) & 15) >> 1)) / 7) * 56)) + (((((((int)blockIdx.x) % 13) * 2) + (((int)threadIdx.x) & 15)) % 14) * 2))] : 0.000000e+00f);
    kernel_pad_shared[((int)threadIdx.x)] = kernel[(((((((int)blockIdx.x) / 13) * 16384) + ((((int)threadIdx.x) >> 4) * 512)) + (ra_fused0_outer * 16)) + (((int)threadIdx.x) & 15))];
    kernel_pad_shared[(((int)threadIdx.x) + 256)] = kernel[((((((((int)blockIdx.x) / 13) * 16384) + ((((int)threadIdx.x) >> 4) * 512)) + (ra_fused0_outer * 16)) + (((int)threadIdx.x) & 15)) + 8192)];
    __syncthreads();
    for (int ra_fused0_inner_outer = 0; ra_fused0_inner_outer < 16; ++ra_fused0_inner_outer) {
      data_pad_shared_local[0] = data_pad_shared[((ra_fused0_inner_outer * 16) + (((int)threadIdx.x) & 15))];
      kernel_pad_shared_local[0] = kernel_pad_shared[(((((int)threadIdx.x) >> 4) * 16) + ra_fused0_inner_outer)];
      kernel_pad_shared_local[1] = kernel_pad_shared[((((((int)threadIdx.x) >> 4) * 16) + ra_fused0_inner_outer) + 256)];
      conv_local[0] = (conv_local[0] + (data_pad_shared_local[0] * kernel_pad_shared_local[0]));
      conv_local[1] = (conv_local[1] + (data_pad_shared_local[0] * kernel_pad_shared_local[1]));
    }
  }
  if ((((((int)blockIdx.x) % 13) * 4) + ((((int)threadIdx.x) & 15) >> 2)) < 49) {
    conv_unpad[(((((((int)blockIdx.x) / 13) * 6272) + ((((int)threadIdx.x) >> 4) * 196)) + ((((int)blockIdx.x) % 13) * 16)) + (((int)threadIdx.x) & 15))] = conv_local[0];
    conv_unpad[((((((((int)blockIdx.x) / 13) * 6272) + ((((int)threadIdx.x) >> 4) * 196)) + ((((int)blockIdx.x) % 13) * 16)) + (((int)threadIdx.x) & 15)) + 3136)] = conv_local[1];
  }
}

dim3 grid(416, 1, 1);
dim3 block(256, 1, 1);
best_idx 4