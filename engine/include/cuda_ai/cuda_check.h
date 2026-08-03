#pragma once

#include <cuda_runtime_api.h>

#include <cstdlib>
#include <iostream>

#define CUDA_AI_CHECK(expression)                                          \
  do {                                                                     \
    const cudaError_t cuda_ai_error = (expression);                        \
    if (cuda_ai_error != cudaSuccess) {                                    \
      std::cerr << "CUDA error at " << __FILE__ << ':' << __LINE__ << ": " \
                << cudaGetErrorString(cuda_ai_error) << '\n';              \
      std::abort();                                                        \
    }                                                                      \
  } while (false)
