#include <algorithm>
#include <cmath>
#include <iostream>
#include <vector>

#include "cuda_ai/resize.h"

#ifdef CUDA_AI_HAS_CUDA
#include "cuda_ai/cuda_check.h"
#include "cuda_ai/resize_cuda.h"
#endif

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
  cuda_ai::resize_bilinear_cpu(
      const_view(source, source_width, source_height, channels),
      view(cpu_output, destination_width, destination_height, channels));
  std::cout << "CPU resize complete; first=" << cpu_output.front()
            << ", last=" << cpu_output.back() << '\n';

#ifdef CUDA_AI_HAS_CUDA
  float* device_source = nullptr;
  float* device_destination = nullptr;
  std::vector<float> gpu_output(cpu_output.size());
  CUDA_AI_CHECK(cudaMalloc(&device_source, source.size() * sizeof(float)));
  CUDA_AI_CHECK(cudaMalloc(&device_destination, gpu_output.size() * sizeof(float)));
  CUDA_AI_CHECK(cudaMemcpy(device_source, source.data(), source.size() * sizeof(float),
                           cudaMemcpyHostToDevice));

  const cuda_ai::ConstImageView device_source_view{
      device_source, source_width, source_height, channels,
      cuda_ai::packed_stride(source_width, channels)};
  const cuda_ai::ImageView device_destination_view{
      device_destination, destination_width, destination_height, channels,
      cuda_ai::packed_stride(destination_width, channels)};
  CUDA_AI_CHECK(
      cuda_ai::resize_bilinear_cuda(device_source_view, device_destination_view));
  CUDA_AI_CHECK(cudaMemcpy(gpu_output.data(), device_destination,
                           gpu_output.size() * sizeof(float), cudaMemcpyDeviceToHost));

  float max_error = 0.0F;
  for (std::size_t i = 0; i < gpu_output.size(); ++i) {
    max_error = std::max(max_error, std::abs(gpu_output[i] - cpu_output[i]));
  }
  std::cout << "CUDA resize complete; max CPU/GPU error=" << max_error << '\n';
  CUDA_AI_CHECK(cudaFree(device_destination));
  CUDA_AI_CHECK(cudaFree(device_source));
#else
  std::cout << "CUDA target unavailable; CPU-only demo completed.\n";
#endif
  return 0;
}
