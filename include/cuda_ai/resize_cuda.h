#pragma once

#include <cuda_runtime_api.h>

#include "cuda_ai/image.h"

namespace cuda_ai {

// Device pointers are required. The operation is enqueued on stream and does
// not synchronize it. The caller owns all memory and the stream.
cudaError_t resize_bilinear_cuda(ConstImageView source, ImageView destination,
                                 cudaStream_t stream = nullptr);

}  // namespace cuda_ai
