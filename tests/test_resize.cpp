#include <cmath>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

#include "cuda_ai/resize.h"

namespace {

cuda_ai::ConstImageView const_view(const std::vector<float>& data, int width,
                                   int height, int channels) {
  return {data.data(), width, height, channels,
          cuda_ai::packed_stride(width, channels)};
}

cuda_ai::ImageView view(std::vector<float>& data, int width, int height, int channels) {
  return {data.data(), width, height, channels,
          cuda_ai::packed_stride(width, channels)};
}

void expect_near(float actual, float expected, float tolerance,
                 const std::string& label) {
  if (std::abs(actual - expected) > tolerance) {
    throw std::runtime_error(label + ": expected " + std::to_string(expected) +
                             ", got " + std::to_string(actual));
  }
}

void test_identity() {
  const std::vector<float> source{1.0F, 2.0F, 3.0F, 4.0F};
  std::vector<float> destination(source.size(), 0.0F);
  cuda_ai::resize_bilinear_cpu(const_view(source, 2, 2, 1), view(destination, 2, 2, 1));
  for (std::size_t i = 0; i < source.size(); ++i) {
    expect_near(destination[i], source[i], 1e-6F, "identity");
  }
}

void test_downscale_to_average() {
  const std::vector<float> source{1.0F, 2.0F, 3.0F, 4.0F};
  std::vector<float> destination(1, 0.0F);
  cuda_ai::resize_bilinear_cpu(const_view(source, 2, 2, 1), view(destination, 1, 1, 1));
  expect_near(destination[0], 2.5F, 1e-6F, "2x2 to 1x1");
}

void test_constant_multichannel_and_odd_size() {
  constexpr int source_width = 3;
  constexpr int source_height = 5;
  constexpr int channels = 4;
  std::vector<float> source(source_width * source_height * channels, 7.25F);
  std::vector<float> destination(7 * 9 * channels, 0.0F);
  cuda_ai::resize_bilinear_cpu(
      const_view(source, source_width, source_height, channels),
      view(destination, 7, 9, channels));
  for (float value : destination) {
    expect_near(value, 7.25F, 1e-6F, "constant preservation");
  }
}

void test_nearest_known_values() {
  const std::vector<float> source{1.0F, 2.0F, 3.0F, 4.0F};
  std::vector<float> destination(16, 0.0F);
  cuda_ai::resize_nearest_cpu(const_view(source, 2, 2, 1), view(destination, 4, 4, 1));
  const std::vector<float> expected{1, 1, 2, 2, 1, 1, 2, 2, 3, 3, 4, 4, 3, 3, 4, 4};
  for (std::size_t i = 0; i < expected.size(); ++i) {
    expect_near(destination[i], expected[i], 0.0F, "nearest upscale");
  }
}

void test_invalid_input() {
  const std::vector<float> source(4, 1.0F);
  std::vector<float> destination(4, 0.0F);
  bool threw = false;
  try {
    cuda_ai::resize_bilinear_cpu(const_view(source, 2, 2, 1),
                                 view(destination, 2, 2, 2));
  } catch (const std::invalid_argument&) {
    threw = true;
  }
  if (!threw) {
    throw std::runtime_error("channel mismatch must throw");
  }
}

}  // namespace

int main() {
  try {
    test_identity();
    test_downscale_to_average();
    test_constant_multichannel_and_odd_size();
    test_nearest_known_values();
    test_invalid_input();
  } catch (const std::exception& error) {
    std::cerr << "FAILED: " << error.what() << '\n';
    return 1;
  }
  std::cout << "All resize CPU tests passed.\n";
  return 0;
}
