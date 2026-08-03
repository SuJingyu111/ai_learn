#include <algorithm>
#include <cmath>
#include <stdexcept>

#include "cuda_ai/resize.h"

namespace cuda_ai {
namespace {

void validate(ConstImageView source, ImageView destination) {
  if (source.data == nullptr || destination.data == nullptr) {
    throw std::invalid_argument("image data pointer must not be null");
  }
  if (source.width <= 0 || source.height <= 0 || destination.width <= 0 ||
      destination.height <= 0 || source.channels <= 0) {
    throw std::invalid_argument("image dimensions and channels must be positive");
  }
  if (source.channels != destination.channels) {
    throw std::invalid_argument("source and destination channels must match");
  }
  if (source.stride_elements < packed_stride(source.width, source.channels) ||
      destination.stride_elements <
          packed_stride(destination.width, destination.channels)) {
    throw std::invalid_argument("image stride is smaller than a packed row");
  }
}

int clamp_index(int value, int upper_bound) {
  return std::max(0, std::min(value, upper_bound - 1));
}

const float* row(ConstImageView image, int y) {
  return image.data + static_cast<std::ptrdiff_t>(y) * image.stride_elements;
}

float* row(ImageView image, int y) {
  return image.data + static_cast<std::ptrdiff_t>(y) * image.stride_elements;
}

}  // namespace

void resize_nearest_cpu(ConstImageView source, ImageView destination) {
  validate(source, destination);
  const float scale_x = static_cast<float>(source.width) / destination.width;
  const float scale_y = static_cast<float>(source.height) / destination.height;

  for (int y = 0; y < destination.height; ++y) {
    const int source_y =
        clamp_index(static_cast<int>(std::floor((y + 0.5F) * scale_y)), source.height);
    float* destination_row = row(destination, y);
    for (int x = 0; x < destination.width; ++x) {
      const int source_x =
          clamp_index(static_cast<int>(std::floor((x + 0.5F) * scale_x)), source.width);
      for (int channel = 0; channel < destination.channels; ++channel) {
        destination_row[x * destination.channels + channel] =
            row(source, source_y)[source_x * source.channels + channel];
      }
    }
  }
}

void resize_bilinear_cpu(ConstImageView source, ImageView destination) {
  validate(source, destination);
  const float scale_x = static_cast<float>(source.width) / destination.width;
  const float scale_y = static_cast<float>(source.height) / destination.height;

  for (int y = 0; y < destination.height; ++y) {
    const float source_y = (y + 0.5F) * scale_y - 0.5F;
    const int y0_unclamped = static_cast<int>(std::floor(source_y));
    const int y0 = clamp_index(y0_unclamped, source.height);
    const int y1 = clamp_index(y0_unclamped + 1, source.height);
    const float fy = source_y - std::floor(source_y);
    float* destination_row = row(destination, y);

    for (int x = 0; x < destination.width; ++x) {
      const float source_x = (x + 0.5F) * scale_x - 0.5F;
      const int x0_unclamped = static_cast<int>(std::floor(source_x));
      const int x0 = clamp_index(x0_unclamped, source.width);
      const int x1 = clamp_index(x0_unclamped + 1, source.width);
      const float fx = source_x - std::floor(source_x);

      for (int channel = 0; channel < destination.channels; ++channel) {
        const float top_left = row(source, y0)[x0 * source.channels + channel];
        const float top_right = row(source, y0)[x1 * source.channels + channel];
        const float bottom_left = row(source, y1)[x0 * source.channels + channel];
        const float bottom_right = row(source, y1)[x1 * source.channels + channel];
        const float top = top_left + fx * (top_right - top_left);
        const float bottom = bottom_left + fx * (bottom_right - bottom_left);
        destination_row[x * destination.channels + channel] = top + fy * (bottom - top);
      }
    }
  }
}

}  // namespace cuda_ai
