
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
extern "C" __global__ void __launch_bounds__(256) default_function_kernel0(float* __restrict__ data, float* __restrict__ kernel, float* __restrict__ bias, float* __restrict__ conv_unpad) {
  float conv_local[2];
  __shared__ float data_pad_shared[2048];
  __shared__ float kernel_pad_shared[4096];
  float data_pad_shared_local[2];
  float kernel_pad_shared_local[1];
  conv_local[0] = 0.000000e+00f;
  conv_local[1] = 0.000000e+00f;
  for (int ra_fused0_outer = 0; ra_fused0_outer < 2; ++ra_fused0_outer) {
    bool cse_var_1 = (ra_fused0_outer < 1);
    __syncthreads();
    data_pad_shared[((int)threadIdx.x)] = (((0 < ((((int)blockIdx.x) * 2) + ((((ra_fused0_outer * 2) + (((int)threadIdx.x) >> 4)) % 9) / 3))) && (0 < (((((int)threadIdx.x) & 15) * 2) + (((ra_fused0_outer * 2) + (((int)threadIdx.x) >> 4)) % 3)))) ? data[(((((((((ra_fused0_outer * 128) + (((int)threadIdx.x) >> 4)) / 9) * 1024) + (((int)blockIdx.x) * 64)) + (((((ra_fused0_outer * 2) + (((int)threadIdx.x) >> 4)) % 9) / 3) * 32)) + ((((int)threadIdx.x) & 15) * 2)) + (((ra_fused0_outer * 2) + (((int)threadIdx.x) >> 4)) % 3)) - 33)] : 0.000000e+00f);
    data_pad_shared[(((int)threadIdx.x) + 256)] = (((cse_var_1 && (0 < ((((int)blockIdx.x) * 2) + (((((ra_fused0_outer * 2) + (((int)threadIdx.x) >> 4)) + 7) % 9) / 3)))) && (0 < (((((int)threadIdx.x) & 15) * 2) + ((((ra_fused0_outer * 2) + (((int)threadIdx.x) >> 4)) + 1) % 3)))) ? data[((((((((((ra_fused0_outer * 128) + (((int)threadIdx.x) >> 4)) + 16) / 9) * 1024) + (((int)blockIdx.x) * 64)) + ((((((ra_fused0_outer * 2) + (((int)threadIdx.x) >> 4)) + 7) % 9) / 3) * 32)) + ((((int)threadIdx.x) & 15) * 2)) + ((((ra_fused0_outer * 2) + (((int)threadIdx.x) >> 4)) + 1) % 3)) - 33)] : 0.000000e+00f);
    data_pad_shared[(((int)threadIdx.x) + 512)] = (((cse_var_1 && (0 < ((((int)blockIdx.x) * 2) + (((((ra_fused0_outer * 2) + (((int)threadIdx.x) >> 4)) + 5) % 9) / 3)))) && (0 < (((((int)threadIdx.x) & 15) * 2) + ((((ra_fused0_outer * 2) + (((int)threadIdx.x) >> 4)) + 2) % 3)))) ? data[((((((((((ra_fused0_outer * 128) + (((int)threadIdx.x) >> 4)) + 32) / 9) * 1024) + (((int)blockIdx.x) * 64)) + ((((((ra_fused0_outer * 2) + (((int)threadIdx.x) >> 4)) + 5) % 9) / 3) * 32)) + ((((int)threadIdx.x) & 15) * 2)) + ((((ra_fused0_outer * 2) + (((int)threadIdx.x) >> 4)) + 2) % 3)) - 33)] : 0.000000e+00f);
    data_pad_shared[(((int)threadIdx.x) + 768)] = (((cse_var_1 && (0 < ((((int)blockIdx.x) * 2) + (((((ra_fused0_outer * 2) + (((int)threadIdx.x) >> 4)) / 3) + 1) % 3)))) && (0 < (((((int)threadIdx.x) & 15) * 2) + (((ra_fused0_outer * 2) + (((int)threadIdx.x) >> 4)) % 3)))) ? data[((((((((((ra_fused0_outer * 128) + (((int)threadIdx.x) >> 4)) + 48) / 9) * 1024) + (((int)blockIdx.x) * 64)) + ((((((int)threadIdx.x) / 48) + 1) % 3) * 32)) + ((((int)threadIdx.x) & 15) * 2)) + (((ra_fused0_outer * 2) + (((int)threadIdx.x) >> 4)) % 3)) - 33)] : 0.000000e+00f);
    data_pad_shared[(((int)threadIdx.x) + 1024)] = (((cse_var_1 && (0 < ((((int)blockIdx.x) * 2) + (((((ra_fused0_outer * 2) + (((int)threadIdx.x) >> 4)) + 1) % 9) / 3)))) && (0 < (((((int)threadIdx.x) & 15) * 2) + ((((ra_fused0_outer * 2) + (((int)threadIdx.x) >> 4)) + 1) % 3)))) ? data[((((((((((ra_fused0_outer * 128) + (((int)threadIdx.x) >> 4)) + 64) / 9) * 1024) + (((int)blockIdx.x) * 64)) + ((((((ra_fused0_outer * 2) + (((int)threadIdx.x) >> 4)) + 1) % 9) / 3) * 32)) + ((((int)threadIdx.x) & 15) * 2)) + ((((ra_fused0_outer * 2) + (((int)threadIdx.x) >> 4)) + 1) % 3)) - 33)] : 0.000000e+00f);
    data_pad_shared[(((int)threadIdx.x) + 1280)] = (((cse_var_1 && (0 < ((((int)blockIdx.x) * 2) + (((((ra_fused0_outer * 2) + (((int)threadIdx.x) >> 4)) + 8) % 9) / 3)))) && (0 < (((((int)threadIdx.x) & 15) * 2) + ((((ra_fused0_outer * 2) + (((int)threadIdx.x) >> 4)) + 2) % 3)))) ? data[((((((((((ra_fused0_outer * 128) + (((int)threadIdx.x) >> 4)) + 80) / 9) * 1024) + (((int)blockIdx.x) * 64)) + ((((((ra_fused0_outer * 2) + (((int)threadIdx.x) >> 4)) + 8) % 9) / 3) * 32)) + ((((int)threadIdx.x) & 15) * 2)) + ((((ra_fused0_outer * 2) + (((int)threadIdx.x) >> 4)) + 2) % 3)) - 33)] : 0.000000e+00f);
    data_pad_shared[(((int)threadIdx.x) + 1536)] = (((cse_var_1 && (0 < ((((int)blockIdx.x) * 2) + (((((ra_fused0_outer * 2) + (((int)threadIdx.x) >> 4)) / 3) + 2) % 3)))) && (0 < (((((int)threadIdx.x) & 15) * 2) + (((ra_fused0_outer * 2) + (((int)threadIdx.x) >> 4)) % 3)))) ? data[((((((((((ra_fused0_outer * 128) + (((int)threadIdx.x) >> 4)) + 96) / 9) * 1024) + (((int)blockIdx.x) * 64)) + ((((((int)threadIdx.x) / 48) + 2) % 3) * 32)) + ((((int)threadIdx.x) & 15) * 2)) + (((ra_fused0_outer * 2) + (((int)threadIdx.x) >> 4)) % 3)) - 33)] : 0.000000e+00f);
    data_pad_shared[(((int)threadIdx.x) + 1792)] = (((cse_var_1 && (0 < ((((int)blockIdx.x) * 2) + (((((ra_fused0_outer * 2) + (((int)threadIdx.x) >> 4)) + 4) % 9) / 3)))) && (0 < (((((int)threadIdx.x) & 15) * 2) + ((((ra_fused0_outer * 2) + (((int)threadIdx.x) >> 4)) + 1) % 3)))) ? data[((((((((((ra_fused0_outer * 128) + (((int)threadIdx.x) >> 4)) + 112) / 9) * 1024) + (((int)blockIdx.x) * 64)) + ((((((ra_fused0_outer * 2) + (((int)threadIdx.x) >> 4)) + 4) % 9) / 3) * 32)) + ((((int)threadIdx.x) & 15) * 2)) + ((((ra_fused0_outer * 2) + (((int)threadIdx.x) >> 4)) + 1) % 3)) - 33)] : 0.000000e+00f);
    kernel_pad_shared[((int)threadIdx.x)] = ((((ra_fused0_outer * 8) + ((((int)threadIdx.x) & 127) >> 4)) < 9) ? kernel[((((((int)threadIdx.x) >> 7) * 144) + ((((ra_fused0_outer * 128) + (((int)threadIdx.x) & 127)) / 9) * 9)) + (((ra_fused0_outer * 2) + (((int)threadIdx.x) & 127)) % 9))] : 0.000000e+00f);
    kernel_pad_shared[(((int)threadIdx.x) + 256)] = ((((ra_fused0_outer * 8) + ((((int)threadIdx.x) & 127) >> 4)) < 9) ? kernel[(((((((int)threadIdx.x) >> 7) * 144) + ((((ra_fused0_outer * 128) + (((int)threadIdx.x) & 127)) / 9) * 9)) + (((ra_fused0_outer * 2) + (((int)threadIdx.x) & 127)) % 9)) + 288)] : 0.000000e+00f);
    kernel_pad_shared[(((int)threadIdx.x) + 512)] = ((((ra_fused0_outer * 8) + ((((int)threadIdx.x) & 127) >> 4)) < 9) ? kernel[(((((((int)threadIdx.x) >> 7) * 144) + ((((ra_fused0_outer * 128) + (((int)threadIdx.x) & 127)) / 9) * 9)) + (((ra_fused0_outer * 2) + (((int)threadIdx.x) & 127)) % 9)) + 576)] : 0.000000e+00f);
    kernel_pad_shared[(((int)threadIdx.x) + 768)] = ((((ra_fused0_outer * 8) + ((((int)threadIdx.x) & 127) >> 4)) < 9) ? kernel[(((((((int)threadIdx.x) >> 7) * 144) + ((((ra_fused0_outer * 128) + (((int)threadIdx.x) & 127)) / 9) * 9)) + (((ra_fused0_outer * 2) + (((int)threadIdx.x) & 127)) % 9)) + 864)] : 0.000000e+00f);
    kernel_pad_shared[(((int)threadIdx.x) + 1024)] = ((((ra_fused0_outer * 8) + ((((int)threadIdx.x) & 127) >> 4)) < 9) ? kernel[(((((((int)threadIdx.x) >> 7) * 144) + ((((ra_fused0_outer * 128) + (((int)threadIdx.x) & 127)) / 9) * 9)) + (((ra_fused0_outer * 2) + (((int)threadIdx.x) & 127)) % 9)) + 1152)] : 0.000000e+00f);
    kernel_pad_shared[(((int)threadIdx.x) + 1280)] = ((((ra_fused0_outer * 8) + ((((int)threadIdx.x) & 127) >> 4)) < 9) ? kernel[(((((((int)threadIdx.x) >> 7) * 144) + ((((ra_fused0_outer * 128) + (((int)threadIdx.x) & 127)) / 9) * 9)) + (((ra_fused0_outer * 2) + (((int)threadIdx.x) & 127)) % 9)) + 1440)] : 0.000000e+00f);
    kernel_pad_shared[(((int)threadIdx.x) + 1536)] = ((((ra_fused0_outer * 8) + ((((int)threadIdx.x) & 127) >> 4)) < 9) ? kernel[(((((((int)threadIdx.x) >> 7) * 144) + ((((ra_fused0_outer * 128) + (((int)threadIdx.x) & 127)) / 9) * 9)) + (((ra_fused0_outer * 2) + (((int)threadIdx.x) & 127)) % 9)) + 1728)] : 0.000000e+00f);
    kernel_pad_shared[(((int)threadIdx.x) + 1792)] = ((((ra_fused0_outer * 8) + ((((int)threadIdx.x) & 127) >> 4)) < 9) ? kernel[(((((((int)threadIdx.x) >> 7) * 144) + ((((ra_fused0_outer * 128) + (((int)threadIdx.x) & 127)) / 9) * 9)) + (((ra_fused0_outer * 2) + (((int)threadIdx.x) & 127)) % 9)) + 2016)] : 0.000000e+00f);
    kernel_pad_shared[(((int)threadIdx.x) + 2048)] = ((((ra_fused0_outer * 8) + ((((int)threadIdx.x) & 127) >> 4)) < 9) ? kernel[(((((((int)threadIdx.x) >> 7) * 144) + ((((ra_fused0_outer * 128) + (((int)threadIdx.x) & 127)) / 9) * 9)) + (((ra_fused0_outer * 2) + (((int)threadIdx.x) & 127)) % 9)) + 2304)] : 0.000000e+00f);
    kernel_pad_shared[(((int)threadIdx.x) + 2304)] = ((((ra_fused0_outer * 8) + ((((int)threadIdx.x) & 127) >> 4)) < 9) ? kernel[(((((((int)threadIdx.x) >> 7) * 144) + ((((ra_fused0_outer * 128) + (((int)threadIdx.x) & 127)) / 9) * 9)) + (((ra_fused0_outer * 2) + (((int)threadIdx.x) & 127)) % 9)) + 2592)] : 0.000000e+00f);
    kernel_pad_shared[(((int)threadIdx.x) + 2560)] = ((((ra_fused0_outer * 8) + ((((int)threadIdx.x) & 127) >> 4)) < 9) ? kernel[(((((((int)threadIdx.x) >> 7) * 144) + ((((ra_fused0_outer * 128) + (((int)threadIdx.x) & 127)) / 9) * 9)) + (((ra_fused0_outer * 2) + (((int)threadIdx.x) & 127)) % 9)) + 2880)] : 0.000000e+00f);
    kernel_pad_shared[(((int)threadIdx.x) + 2816)] = ((((ra_fused0_outer * 8) + ((((int)threadIdx.x) & 127) >> 4)) < 9) ? kernel[(((((((int)threadIdx.x) >> 7) * 144) + ((((ra_fused0_outer * 128) + (((int)threadIdx.x) & 127)) / 9) * 9)) + (((ra_fused0_outer * 2) + (((int)threadIdx.x) & 127)) % 9)) + 3168)] : 0.000000e+00f);
    kernel_pad_shared[(((int)threadIdx.x) + 3072)] = ((((ra_fused0_outer * 8) + ((((int)threadIdx.x) & 127) >> 4)) < 9) ? kernel[(((((((int)threadIdx.x) >> 7) * 144) + ((((ra_fused0_outer * 128) + (((int)threadIdx.x) & 127)) / 9) * 9)) + (((ra_fused0_outer * 2) + (((int)threadIdx.x) & 127)) % 9)) + 3456)] : 0.000000e+00f);
    kernel_pad_shared[(((int)threadIdx.x) + 3328)] = ((((ra_fused0_outer * 8) + ((((int)threadIdx.x) & 127) >> 4)) < 9) ? kernel[(((((((int)threadIdx.x) >> 7) * 144) + ((((ra_fused0_outer * 128) + (((int)threadIdx.x) & 127)) / 9) * 9)) + (((ra_fused0_outer * 2) + (((int)threadIdx.x) & 127)) % 9)) + 3744)] : 0.000000e+00f);
    kernel_pad_shared[(((int)threadIdx.x) + 3584)] = ((((ra_fused0_outer * 8) + ((((int)threadIdx.x) & 127) >> 4)) < 9) ? kernel[(((((((int)threadIdx.x) >> 7) * 144) + ((((ra_fused0_outer * 128) + (((int)threadIdx.x) & 127)) / 9) * 9)) + (((ra_fused0_outer * 2) + (((int)threadIdx.x) & 127)) % 9)) + 4032)] : 0.000000e+00f);
    kernel_pad_shared[(((int)threadIdx.x) + 3840)] = ((((ra_fused0_outer * 8) + ((((int)threadIdx.x) & 127) >> 4)) < 9) ? kernel[(((((((int)threadIdx.x) >> 7) * 144) + ((((ra_fused0_outer * 128) + (((int)threadIdx.x) & 127)) / 9) * 9)) + (((ra_fused0_outer * 2) + (((int)threadIdx.x) & 127)) % 9)) + 4320)] : 0.000000e+00f);
    __syncthreads();
    for (int ra_fused0_inner_outer = 0; ra_fused0_inner_outer < 128; ++ra_fused0_inner_outer) {
      data_pad_shared_local[0] = data_pad_shared[((ra_fused0_inner_outer * 16) + (((int)threadIdx.x) & 7))];
      data_pad_shared_local[1] = data_pad_shared[(((ra_fused0_inner_outer * 16) + (((int)threadIdx.x) & 7)) + 8)];
      kernel_pad_shared_local[0] = kernel_pad_shared[(((((int)threadIdx.x) >> 3) * 128) + ra_fused0_inner_outer)];
      if (((ra_fused0_outer * 8) + (ra_fused0_inner_outer >> 4)) < 9) {
        conv_local[0] = (conv_local[0] + (data_pad_shared_local[0] * kernel_pad_shared_local[0]));
        conv_local[1] = (conv_local[1] + (data_pad_shared_local[1] * kernel_pad_shared_local[0]));
      }
    }
  }
  conv_unpad[((((((int)threadIdx.x) >> 3) * 256) + (((int)blockIdx.x) * 16)) + (((int)threadIdx.x) & 7))] = max((conv_local[0] + bias[(((int)threadIdx.x) >> 3)]), 0.000000e+00f);
  conv_unpad[(((((((int)threadIdx.x) >> 3) * 256) + (((int)blockIdx.x) * 16)) + (((int)threadIdx.x) & 7)) + 8)] = max((conv_local[1] + bias[(((int)threadIdx.x) >> 3)]), 0.000000e+00f);
}

dim3 grid(16, 1, 1);
dim3 block(256, 1, 1);
best_idx 10