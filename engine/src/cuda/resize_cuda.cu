#include <cuda_runtime.h>

#include "cuda_ai/resize_cuda.h"

namespace cuda_ai {
namespace {

__device__ int clamp_index(int value, int upper_bound) {
  return max(0, min(value, upper_bound - 1));
}

__global__ void resize_bilinear_kernel(ConstImageView source, ImageView destination) {
  const int x = blockIdx.x * blockDim.x + threadIdx.x;
  const int y = blockIdx.y * blockDim.y + threadIdx.y;
  if (x >= destination.width || y >= destination.height) {
    return;
  }

  const float scale_x = static_cast<float>(source.width) / destination.width;
  const float scale_y = static_cast<float>(source.height) / destination.height;
  const float source_x = (x + 0.5F) * scale_x - 0.5F;
  const float source_y = (y + 0.5F) * scale_y - 0.5F;
  const int raw_x0 = static_cast<int>(floorf(source_x));
  const int raw_y0 = static_cast<int>(floorf(source_y));
  const int x0 = clamp_index(raw_x0, source.width);
  const int x1 = clamp_index(raw_x0 + 1, source.width);
  const int y0 = clamp_index(raw_y0, source.height);
  const int y1 = clamp_index(raw_y0 + 1, source.height);
  const float fx = source_x - floorf(source_x);
  const float fy = source_y - floorf(source_y);

  const float* row0 =
      source.data + static_cast<std::ptrdiff_t>(y0) * source.stride_elements;
  const float* row1 =
      source.data + static_cast<std::ptrdiff_t>(y1) * source.stride_elements;
  float* output = destination.data +
                  static_cast<std::ptrdiff_t>(y) * destination.stride_elements +
                  x * destination.channels;

  for (int channel = 0; channel < destination.channels; ++channel) {
    const float top_left = row0[x0 * source.channels + channel];
    const float top_right = row0[x1 * source.channels + channel];
    const float bottom_left = row1[x0 * source.channels + channel];
    const float bottom_right = row1[x1 * source.channels + channel];
    const float top = top_left + fx * (top_right - top_left);
    const float bottom = bottom_left + fx * (bottom_right - bottom_left);
    output[channel] = top + fy * (bottom - top);
  }
}

bool invalid(ConstImageView source, ImageView destination) {
  return source.data == nullptr || destination.data == nullptr || source.width <= 0 ||
         source.height <= 0 || destination.width <= 0 || destination.height <= 0 ||
         source.channels <= 0 || source.channels != destination.channels ||
         source.stride_elements < packed_stride(source.width, source.channels) ||
         destination.stride_elements <
             packed_stride(destination.width, destination.channels);
}

}  // namespace

cudaError_t resize_bilinear_cuda(ConstImageView source, ImageView destination,
                                 cudaStream_t stream) {
  if (invalid(source, destination)) {
    return cudaErrorInvalidValue;
  }
  const dim3 block(32, 8);
  const dim3 grid((destination.width + block.x - 1) / block.x,
                  (destination.height + block.y - 1) / block.y);
  resize_bilinear_kernel<<<grid, block, 0, stream>>>(source, destination);
  return cudaGetLastError();
}

}  // namespace cuda_ai
