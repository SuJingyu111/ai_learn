#pragma once

// Shared helpers for the CPU and CUDA resize suites. Plain C++17 so the same
// header compiles under both the host compiler and nvcc.

#include <cstddef>
#include <random>
#include <stdexcept>
#include <string>
#include <vector>

#include "tt/image.h"

namespace tt_test {

// A host image whose row stride may exceed a packed row. The padding is filled
// with a sentinel so a test can prove nothing wrote past the logical width.
struct HostImage {
  std::vector<float> storage;
  int width{0};
  int height{0};
  int channels{0};
  std::ptrdiff_t stride_elements{0};

  static constexpr float kPaddingSentinel = -12345.0F;

  HostImage(int image_width, int image_height, int image_channels,
            std::ptrdiff_t extra_stride_elements = 0)
      : width(image_width),
        height(image_height),
        channels(image_channels),
        stride_elements(tt::packed_stride(image_width, image_channels) +
                        extra_stride_elements) {
    storage.assign(static_cast<std::size_t>(stride_elements) * image_height,
                   kPaddingSentinel);
  }

  tt::ConstImageView const_view() const {
    return {storage.data(), width, height, channels, stride_elements};
  }

  tt::ImageView view() {
    return {storage.data(), width, height, channels, stride_elements};
  }

  float* row(int y) {
    return storage.data() + static_cast<std::ptrdiff_t>(y) * stride_elements;
  }

  const float* row(int y) const {
    return storage.data() + static_cast<std::ptrdiff_t>(y) * stride_elements;
  }

  float at(int y, int x, int channel) const { return row(y)[x * channels + channel]; }

  // Fills only the logical pixels, leaving stride padding at the sentinel.
  void fill_random(unsigned seed) {
    std::mt19937 generator(seed);
    std::uniform_real_distribution<float> distribution(0.0F, 1.0F);
    for (int y = 0; y < height; ++y) {
      float* destination = row(y);
      for (int x = 0; x < width * channels; ++x) {
        destination[x] = distribution(generator);
      }
    }
  }

  void fill_constant(float value) {
    for (int y = 0; y < height; ++y) {
      float* destination = row(y);
      for (int x = 0; x < width * channels; ++x) {
        destination[x] = value;
      }
    }
  }

  // True when every element between the logical row end and the next row start
  // still holds the sentinel, i.e. the operation never wrote out of bounds.
  bool padding_intact() const {
    const std::ptrdiff_t packed = tt::packed_stride(width, channels);
    for (int y = 0; y < height; ++y) {
      const float* source = row(y);
      for (std::ptrdiff_t x = packed; x < stride_elements; ++x) {
        if (source[x] != kPaddingSentinel) {
          return false;
        }
      }
    }
    return true;
  }
};

struct ResizeCase {
  const char* name;
  int source_width;
  int source_height;
  int destination_width;
  int destination_height;
  int channels;
};

// Sizes chosen so the CUDA launch geometry (32x8 threads) is exercised on and
// off its block multiples, including degenerate single-row/column/pixel
// images. docs/architecture.md requires odd, single-pixel and boundary cases.
inline std::vector<ResizeCase> resize_cases() {
  return {
      {"identity", 8, 8, 8, 8, 1},
      {"block-multiple-control", 64, 16, 32, 8, 1},
      {"odd-non-block-multiple", 33, 9, 37, 11, 1},
      {"single-pixel-source", 1, 1, 13, 7, 3},
      {"single-pixel-destination", 23, 17, 1, 1, 1},
      {"single-column", 1, 37, 5, 41, 1},
      {"single-row", 41, 1, 47, 3, 4},
      {"upscale-rgb", 7, 5, 19, 13, 3},
      {"downscale-rgba", 100, 60, 33, 17, 4},
      {"wide-non-multiple", 129, 3, 65, 5, 1},
  };
}

inline void check(bool condition, const std::string& label) {
  if (!condition) {
    throw std::runtime_error(label);
  }
}

}  // namespace tt_test
