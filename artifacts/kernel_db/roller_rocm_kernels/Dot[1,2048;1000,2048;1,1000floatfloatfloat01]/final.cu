
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
extern "C" __global__ void __launch_bounds__(64) default_function_kernel0(float* __restrict__ A, float* __restrict__ B, float* __restrict__ compute) {
  float compute_local[2];
  __shared__ float A_shared[64];
  __shared__ float B_shared[4096];
  float A_shared_local[2];
  float B_shared_local[1];
  compute_local[0] = 0.000000e+00f;
  compute_local[1] = 0.000000e+00f;
  for (int k_outer = 0; k_outer < 32; ++k_outer) {
    __syncthreads();
    A_shared[((int)threadIdx.x)] = A[((k_outer * 64) + ((int)threadIdx.x))];
    B_shared[((int)threadIdx.x)] = B[(((((int)blockIdx.x) * 131072) + (k_outer * 64)) + ((int)threadIdx.x))];
    B_shared[(((int)threadIdx.x) + 64)] = B[((((((int)blockIdx.x) * 131072) + (k_outer * 64)) + ((int)threadIdx.x)) + 2048)];
    B_shared[(((int)threadIdx.x) + 128)] = B[((((((int)blockIdx.x) * 131072) + (k_outer * 64)) + ((int)threadIdx.x)) + 4096)];
    B_shared[(((int)threadIdx.x) + 192)] = B[((((((int)blockIdx.x) * 131072) + (k_outer * 64)) + ((int)threadIdx.x)) + 6144)];
    B_shared[(((int)threadIdx.x) + 256)] = B[((((((int)blockIdx.x) * 131072) + (k_outer * 64)) + ((int)threadIdx.x)) + 8192)];
    B_shared[(((int)threadIdx.x) + 320)] = B[((((((int)blockIdx.x) * 131072) + (k_outer * 64)) + ((int)threadIdx.x)) + 10240)];
    B_shared[(((int)threadIdx.x) + 384)] = B[((((((int)blockIdx.x) * 131072) + (k_outer * 64)) + ((int)threadIdx.x)) + 12288)];
    B_shared[(((int)threadIdx.x) + 448)] = B[((((((int)blockIdx.x) * 131072) + (k_outer * 64)) + ((int)threadIdx.x)) + 14336)];
    B_shared[(((int)threadIdx.x) + 512)] = B[((((((int)blockIdx.x) * 131072) + (k_outer * 64)) + ((int)threadIdx.x)) + 16384)];
    B_shared[(((int)threadIdx.x) + 576)] = B[((((((int)blockIdx.x) * 131072) + (k_outer * 64)) + ((int)threadIdx.x)) + 18432)];
    B_shared[(((int)threadIdx.x) + 640)] = B[((((((int)blockIdx.x) * 131072) + (k_outer * 64)) + ((int)threadIdx.x)) + 20480)];
    B_shared[(((int)threadIdx.x) + 704)] = B[((((((int)blockIdx.x) * 131072) + (k_outer * 64)) + ((int)threadIdx.x)) + 22528)];
    B_shared[(((int)threadIdx.x) + 768)] = B[((((((int)blockIdx.x) * 131072) + (k_outer * 64)) + ((int)threadIdx.x)) + 24576)];
    B_shared[(((int)threadIdx.x) + 832)] = B[((((((int)blockIdx.x) * 131072) + (k_outer * 64)) + ((int)threadIdx.x)) + 26624)];
    B_shared[(((int)threadIdx.x) + 896)] = B[((((((int)blockIdx.x) * 131072) + (k_outer * 64)) + ((int)threadIdx.x)) + 28672)];
    B_shared[(((int)threadIdx.x) + 960)] = B[((((((int)blockIdx.x) * 131072) + (k_outer * 64)) + ((int)threadIdx.x)) + 30720)];
    B_shared[(((int)threadIdx.x) + 1024)] = B[((((((int)blockIdx.x) * 131072) + (k_outer * 64)) + ((int)threadIdx.x)) + 32768)];
    B_shared[(((int)threadIdx.x) + 1088)] = B[((((((int)blockIdx.x) * 131072) + (k_outer * 64)) + ((int)threadIdx.x)) + 34816)];
    B_shared[(((int)threadIdx.x) + 1152)] = B[((((((int)blockIdx.x) * 131072) + (k_outer * 64)) + ((int)threadIdx.x)) + 36864)];
    B_shared[(((int)threadIdx.x) + 1216)] = B[((((((int)blockIdx.x) * 131072) + (k_outer * 64)) + ((int)threadIdx.x)) + 38912)];
    B_shared[(((int)threadIdx.x) + 1280)] = B[((((((int)blockIdx.x) * 131072) + (k_outer * 64)) + ((int)threadIdx.x)) + 40960)];
    B_shared[(((int)threadIdx.x) + 1344)] = B[((((((int)blockIdx.x) * 131072) + (k_outer * 64)) + ((int)threadIdx.x)) + 43008)];
    B_shared[(((int)threadIdx.x) + 1408)] = B[((((((int)blockIdx.x) * 131072) + (k_outer * 64)) + ((int)threadIdx.x)) + 45056)];
    B_shared[(((int)threadIdx.x) + 1472)] = B[((((((int)blockIdx.x) * 131072) + (k_outer * 64)) + ((int)threadIdx.x)) + 47104)];
    B_shared[(((int)threadIdx.x) + 1536)] = B[((((((int)blockIdx.x) * 131072) + (k_outer * 64)) + ((int)threadIdx.x)) + 49152)];
    B_shared[(((int)threadIdx.x) + 1600)] = B[((((((int)blockIdx.x) * 131072) + (k_outer * 64)) + ((int)threadIdx.x)) + 51200)];
    B_shared[(((int)threadIdx.x) + 1664)] = B[((((((int)blockIdx.x) * 131072) + (k_outer * 64)) + ((int)threadIdx.x)) + 53248)];
    B_shared[(((int)threadIdx.x) + 1728)] = B[((((((int)blockIdx.x) * 131072) + (k_outer * 64)) + ((int)threadIdx.x)) + 55296)];
    B_shared[(((int)threadIdx.x) + 1792)] = B[((((((int)blockIdx.x) * 131072) + (k_outer * 64)) + ((int)threadIdx.x)) + 57344)];
    B_shared[(((int)threadIdx.x) + 1856)] = B[((((((int)blockIdx.x) * 131072) + (k_outer * 64)) + ((int)threadIdx.x)) + 59392)];
    B_shared[(((int)threadIdx.x) + 1920)] = B[((((((int)blockIdx.x) * 131072) + (k_outer * 64)) + ((int)threadIdx.x)) + 61440)];
    B_shared[(((int)threadIdx.x) + 1984)] = B[((((((int)blockIdx.x) * 131072) + (k_outer * 64)) + ((int)threadIdx.x)) + 63488)];
    B_shared[(((int)threadIdx.x) + 2048)] = B[((((((int)blockIdx.x) * 131072) + (k_outer * 64)) + ((int)threadIdx.x)) + 65536)];
    B_shared[(((int)threadIdx.x) + 2112)] = B[((((((int)blockIdx.x) * 131072) + (k_outer * 64)) + ((int)threadIdx.x)) + 67584)];
    B_shared[(((int)threadIdx.x) + 2176)] = B[((((((int)blockIdx.x) * 131072) + (k_outer * 64)) + ((int)threadIdx.x)) + 69632)];
    B_shared[(((int)threadIdx.x) + 2240)] = B[((((((int)blockIdx.x) * 131072) + (k_outer * 64)) + ((int)threadIdx.x)) + 71680)];
    B_shared[(((int)threadIdx.x) + 2304)] = B[((((((int)blockIdx.x) * 131072) + (k_outer * 64)) + ((int)threadIdx.x)) + 73728)];
    B_shared[(((int)threadIdx.x) + 2368)] = B[((((((int)blockIdx.x) * 131072) + (k_outer * 64)) + ((int)threadIdx.x)) + 75776)];
    B_shared[(((int)threadIdx.x) + 2432)] = B[((((((int)blockIdx.x) * 131072) + (k_outer * 64)) + ((int)threadIdx.x)) + 77824)];
    B_shared[(((int)threadIdx.x) + 2496)] = B[((((((int)blockIdx.x) * 131072) + (k_outer * 64)) + ((int)threadIdx.x)) + 79872)];
    if (((int)blockIdx.x) < 15) {
      B_shared[(((int)threadIdx.x) + 2560)] = B[((((((int)blockIdx.x) * 131072) + (k_outer * 64)) + ((int)threadIdx.x)) + 81920)];
    }
    if (((int)blockIdx.x) < 15) {
      B_shared[(((int)threadIdx.x) + 2624)] = B[((((((int)blockIdx.x) * 131072) + (k_outer * 64)) + ((int)threadIdx.x)) + 83968)];
    }
    if (((int)blockIdx.x) < 15) {
      B_shared[(((int)threadIdx.x) + 2688)] = B[((((((int)blockIdx.x) * 131072) + (k_outer * 64)) + ((int)threadIdx.x)) + 86016)];
    }
    if (((int)blockIdx.x) < 15) {
      B_shared[(((int)threadIdx.x) + 2752)] = B[((((((int)blockIdx.x) * 131072) + (k_outer * 64)) + ((int)threadIdx.x)) + 88064)];
    }
    if (((int)blockIdx.x) < 15) {
      B_shared[(((int)threadIdx.x) + 2816)] = B[((((((int)blockIdx.x) * 131072) + (k_outer * 64)) + ((int)threadIdx.x)) + 90112)];
    }
    if (((int)blockIdx.x) < 15) {
      B_shared[(((int)threadIdx.x) + 2880)] = B[((((((int)blockIdx.x) * 131072) + (k_outer * 64)) + ((int)threadIdx.x)) + 92160)];
    }
    if (((int)blockIdx.x) < 15) {
      B_shared[(((int)threadIdx.x) + 2944)] = B[((((((int)blockIdx.x) * 131072) + (k_outer * 64)) + ((int)threadIdx.x)) + 94208)];
    }
    if (((int)blockIdx.x) < 15) {
      B_shared[(((int)threadIdx.x) + 3008)] = B[((((((int)blockIdx.x) * 131072) + (k_outer * 64)) + ((int)threadIdx.x)) + 96256)];
    }
    if (((int)blockIdx.x) < 15) {
      B_shared[(((int)threadIdx.x) + 3072)] = B[((((((int)blockIdx.x) * 131072) + (k_outer * 64)) + ((int)threadIdx.x)) + 98304)];
    }
    if (((int)blockIdx.x) < 15) {
      B_shared[(((int)threadIdx.x) + 3136)] = B[((((((int)blockIdx.x) * 131072) + (k_outer * 64)) + ((int)threadIdx.x)) + 100352)];
    }
    if (((int)blockIdx.x) < 15) {
      B_shared[(((int)threadIdx.x) + 3200)] = B[((((((int)blockIdx.x) * 131072) + (k_outer * 64)) + ((int)threadIdx.x)) + 102400)];
    }
    if (((int)blockIdx.x) < 15) {
      B_shared[(((int)threadIdx.x) + 3264)] = B[((((((int)blockIdx.x) * 131072) + (k_outer * 64)) + ((int)threadIdx.x)) + 104448)];
    }
    if (((int)blockIdx.x) < 15) {
      B_shared[(((int)threadIdx.x) + 3328)] = B[((((((int)blockIdx.x) * 131072) + (k_outer * 64)) + ((int)threadIdx.x)) + 106496)];
    }
    if (((int)blockIdx.x) < 15) {
      B_shared[(((int)threadIdx.x) + 3392)] = B[((((((int)blockIdx.x) * 131072) + (k_outer * 64)) + ((int)threadIdx.x)) + 108544)];
    }
    if (((int)blockIdx.x) < 15) {
      B_shared[(((int)threadIdx.x) + 3456)] = B[((((((int)blockIdx.x) * 131072) + (k_outer * 64)) + ((int)threadIdx.x)) + 110592)];
    }
    if (((int)blockIdx.x) < 15) {
      B_shared[(((int)threadIdx.x) + 3520)] = B[((((((int)blockIdx.x) * 131072) + (k_outer * 64)) + ((int)threadIdx.x)) + 112640)];
    }
    if (((int)blockIdx.x) < 15) {
      B_shared[(((int)threadIdx.x) + 3584)] = B[((((((int)blockIdx.x) * 131072) + (k_outer * 64)) + ((int)threadIdx.x)) + 114688)];
    }
    if (((int)blockIdx.x) < 15) {
      B_shared[(((int)threadIdx.x) + 3648)] = B[((((((int)blockIdx.x) * 131072) + (k_outer * 64)) + ((int)threadIdx.x)) + 116736)];
    }
    if (((int)blockIdx.x) < 15) {
      B_shared[(((int)threadIdx.x) + 3712)] = B[((((((int)blockIdx.x) * 131072) + (k_outer * 64)) + ((int)threadIdx.x)) + 118784)];
    }
    if (((int)blockIdx.x) < 15) {
      B_shared[(((int)threadIdx.x) + 3776)] = B[((((((int)blockIdx.x) * 131072) + (k_outer * 64)) + ((int)threadIdx.x)) + 120832)];
    }
    if (((int)blockIdx.x) < 15) {
      B_shared[(((int)threadIdx.x) + 3840)] = B[((((((int)blockIdx.x) * 131072) + (k_outer * 64)) + ((int)threadIdx.x)) + 122880)];
    }
    if (((int)blockIdx.x) < 15) {
      B_shared[(((int)threadIdx.x) + 3904)] = B[((((((int)blockIdx.x) * 131072) + (k_outer * 64)) + ((int)threadIdx.x)) + 124928)];
    }
    if (((int)blockIdx.x) < 15) {
      B_shared[(((int)threadIdx.x) + 3968)] = B[((((((int)blockIdx.x) * 131072) + (k_outer * 64)) + ((int)threadIdx.x)) + 126976)];
    }
    if (((int)blockIdx.x) < 15) {
      B_shared[(((int)threadIdx.x) + 4032)] = B[((((((int)blockIdx.x) * 131072) + (k_outer * 64)) + ((int)threadIdx.x)) + 129024)];
    }
    __syncthreads();
    for (int k_inner_outer = 0; k_inner_outer < 64; ++k_inner_outer) {
      A_shared_local[0] = A_shared[k_inner_outer];
      if (((((int)blockIdx.x) * 8) + (((int)threadIdx.x) >> 3)) < 125) {
        B_shared_local[0] = B_shared[((((int)threadIdx.x) * 64) + k_inner_outer)];
      }
      if (((((int)blockIdx.x) * 8) + (((int)threadIdx.x) >> 3)) < 125) {
        compute_local[0] = (compute_local[0] + (A_shared_local[0] * B_shared_local[0]));
      }
    }
  }
  if (((((int)blockIdx.x) * 8) + (((int)threadIdx.x) >> 3)) < 125) {
    compute[((((int)blockIdx.x) * 64) + ((int)threadIdx.x))] = compute_local[0];
  }
}

dim3 grid(16, 1, 1);
dim3 block(64, 1, 1);
best_idx 8