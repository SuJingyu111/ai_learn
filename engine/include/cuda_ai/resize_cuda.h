#pragma once

#include <cuda_runtime_api.h>

#include "cuda_ai/image.h"

namespace cuda_ai {

// Coordinate, border, dtype and layout semantics are the ones documented in
// cuda_ai/resize.h; resize_bilinear_cpu is the correctness oracle for this
// kernel (see tests/test_resize_cuda.cu).
//
// Device pointers are required. The operation is enqueued on stream and does
// not synchronize it. The caller owns all memory and the stream. Invalid
// arguments return cudaErrorInvalidValue without launching; otherwise the
// return value is cudaGetLastError() for the launch, which cannot report an
// asynchronous execution error -- synchronize to observe those.
cudaError_t resize_bilinear_cuda(ConstImageView source, ImageView destination,
                                 cudaStream_t stream = nullptr);

}  // namespace cuda_ai
