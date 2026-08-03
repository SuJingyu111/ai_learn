#pragma once

#include <cuda_runtime_api.h>

#include <cstdlib>
#include <iostream>

#define TT_CUDA_CHECK(expression)                                          \
  do {                                                                     \
    const cudaError_t tt_cuda_error = (expression);                        \
    if (tt_cuda_error != cudaSuccess) {                                    \
      std::cerr << "CUDA error at " << __FILE__ << ':' << __LINE__ << ": " \
                << cudaGetErrorString(tt_cuda_error) << '\n';              \
      std::abort();                                                        \
    }                                                                      \
  } while (false)
