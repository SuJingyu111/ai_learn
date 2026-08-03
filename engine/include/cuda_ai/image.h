#pragma once

#include <cstddef>

namespace cuda_ai {

struct ConstImageView {
  const float* data{nullptr};
  int width{0};
  int height{0};
  int channels{0};
  std::ptrdiff_t stride_elements{0};
};

struct ImageView {
  float* data{nullptr};
  int width{0};
  int height{0};
  int channels{0};
  std::ptrdiff_t stride_elements{0};
};

inline std::ptrdiff_t packed_stride(int width, int channels) {
  return static_cast<std::ptrdiff_t>(width) * channels;
}

}  // namespace cuda_ai
