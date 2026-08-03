#include <algorithm>
#include <cmath>
#include <iostream>
#include <vector>

#include "tt/resize.h"

#ifdef TT_HAS_CUDA
#include "tt/cuda_check.h"
#include "tt/resize_cuda.h"
#endif

namespace {

tt::ConstImageView const_view(const std::vector<float>& data, int width, int height,
                              int channels) {
  return {data.data(), width, height, channels, tt::packed_stride(width, channels)};
}

tt::ImageView view(std::vector<float>& data, int width, int height, int channels) {
  return {data.data(), width, height, channels, tt::packed_stride(width, channels)};
}

}  // namespace

int main() {
  constexpr int source_width = 4;
  constexpr int source_height = 3;
  constexpr int destination_width = 7;
  constexpr int destination_height = 5;
  constexpr int channels = 1;

  std::vector<float> source(source_width * source_height * channels);
  for (std::size_t i = 0; i < source.size(); ++i) {
    source[i] = static_cast<float>(i);
  }
  std::vector<float> cpu_output(destination_width * destination_height * channels);
  tt::resize_bilinear_cpu(
      const_view(source, source_width, source_height, channels),
      view(cpu_output, destination_width, destination_height, channels));
  std::cout << "CPU resize complete; first=" << cpu_output.front()
            << ", last=" << cpu_output.back() << '\n';

#ifdef TT_HAS_CUDA
  float* device_source = nullptr;
  float* device_destination = nullptr;
  std::vector<float> gpu_output(cpu_output.size());
  TT_CUDA_CHECK(cudaMalloc(&device_source, source.size() * sizeof(float)));
  TT_CUDA_CHECK(cudaMalloc(&device_destination, gpu_output.size() * sizeof(float)));
  TT_CUDA_CHECK(cudaMemcpy(device_source, source.data(), source.size() * sizeof(float),
                           cudaMemcpyHostToDevice));

  const tt::ConstImageView device_source_view{
      device_source, source_width, source_height, channels,
      tt::packed_stride(source_width, channels)};
  const tt::ImageView device_destination_view{
      device_destination, destination_width, destination_height, channels,
      tt::packed_stride(destination_width, channels)};
  TT_CUDA_CHECK(tt::resize_bilinear_cuda(device_source_view, device_destination_view));
  TT_CUDA_CHECK(cudaMemcpy(gpu_output.data(), device_destination,
                           gpu_output.size() * sizeof(float), cudaMemcpyDeviceToHost));

  float max_error = 0.0F;
  for (std::size_t i = 0; i < gpu_output.size(); ++i) {
    max_error = std::max(max_error, std::abs(gpu_output[i] - cpu_output[i]));
  }
  std::cout << "CUDA resize complete; max CPU/GPU error=" << max_error << '\n';
  TT_CUDA_CHECK(cudaFree(device_destination));
  TT_CUDA_CHECK(cudaFree(device_source));
#else
  std::cout << "CUDA target unavailable; CPU-only demo completed.\n";
#endif
  return 0;
}
