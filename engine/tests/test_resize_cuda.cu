// CPU-oracle parity for the CUDA bilinear resize.
//
// The CPU reference in src/cpu/resize_cpu.cpp is the oracle. Both paths use
// the same coordinate, border and layout contract (see include/cuda_ai/resize.h),
// so the only permitted difference is floating-point: nvcc contracts a + b * c
// into FMA, which the host compiler does not, giving roughly one ulp per
// operation. With inputs in [0, 1] and three chained lerps that stays far below
// the 1e-5 absolute tolerance used here.

#include <cuda_runtime.h>

#include <cmath>
#include <cstddef>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

#include "cuda_ai/resize.h"
#include "cuda_ai/resize_cuda.h"
#include "test_support.h"

namespace {

constexpr float kTolerance = 1e-5F;

// ctest reports this as "skipped" rather than failed, via SKIP_RETURN_CODE.
constexpr int kSkipExitCode = 77;

class DeviceBuffer {
 public:
  explicit DeviceBuffer(std::size_t element_count) {
    const cudaError_t status = cudaMalloc(&pointer_, element_count * sizeof(float));
    if (status != cudaSuccess) {
      throw std::runtime_error(std::string("cudaMalloc: ") +
                               cudaGetErrorString(status));
    }
  }
  ~DeviceBuffer() { cudaFree(pointer_); }

  DeviceBuffer(const DeviceBuffer&) = delete;
  DeviceBuffer& operator=(const DeviceBuffer&) = delete;

  float* get() const { return pointer_; }

 private:
  float* pointer_{nullptr};
};

void check_cuda(cudaError_t status, const std::string& label) {
  if (status != cudaSuccess) {
    throw std::runtime_error(label + ": " + cudaGetErrorString(status));
  }
}

// Runs one case on both backends. extra_source_stride / extra_destination_stride
// force a padded row stride; the device destination is pre-filled with the
// sentinel so writes past the logical width are detected rather than ignored.
void compare_case(const cuda_ai_test::ResizeCase& test_case,
                  std::ptrdiff_t extra_source_stride,
                  std::ptrdiff_t extra_destination_stride) {
  cuda_ai_test::HostImage source(test_case.source_width, test_case.source_height,
                                 test_case.channels, extra_source_stride);
  source.fill_random(20260727U);

  cuda_ai_test::HostImage expected(test_case.destination_width,
                                   test_case.destination_height, test_case.channels,
                                   extra_destination_stride);
  cuda_ai::resize_bilinear_cpu(source.const_view(), expected.view());

  cuda_ai_test::HostImage actual(test_case.destination_width,
                                 test_case.destination_height, test_case.channels,
                                 extra_destination_stride);

  DeviceBuffer device_source(source.storage.size());
  DeviceBuffer device_destination(actual.storage.size());
  check_cuda(cudaMemcpy(device_source.get(), source.storage.data(),
                        source.storage.size() * sizeof(float), cudaMemcpyHostToDevice),
             "H2D source");
  // Sentinel-filled, so untouched padding stays detectable after D2H.
  check_cuda(cudaMemcpy(device_destination.get(), actual.storage.data(),
                        actual.storage.size() * sizeof(float), cudaMemcpyHostToDevice),
             "H2D destination");

  const cuda_ai::ConstImageView device_source_view{device_source.get(), source.width,
                                                   source.height, source.channels,
                                                   source.stride_elements};
  const cuda_ai::ImageView device_destination_view{
      device_destination.get(), actual.width, actual.height, actual.channels,
      actual.stride_elements};

  check_cuda(cuda_ai::resize_bilinear_cuda(device_source_view, device_destination_view),
             "resize_bilinear_cuda launch");
  // The launch is asynchronous: without this the D2H below could race, and an
  // execution error would surface at an unrelated call site.
  check_cuda(cudaDeviceSynchronize(), "kernel execution");
  check_cuda(cudaMemcpy(actual.storage.data(), device_destination.get(),
                        actual.storage.size() * sizeof(float), cudaMemcpyDeviceToHost),
             "D2H destination");

  const std::string label = std::string(test_case.name) + " (src pad " +
                            std::to_string(extra_source_stride) + ", dst pad " +
                            std::to_string(extra_destination_stride) + ")";
  cuda_ai_test::check(actual.padding_intact(),
                      label + ": kernel wrote into stride padding");

  float largest_error = 0.0F;
  for (int y = 0; y < test_case.destination_height; ++y) {
    for (int x = 0; x < test_case.destination_width; ++x) {
      for (int c = 0; c < test_case.channels; ++c) {
        const float difference = std::fabs(actual.at(y, x, c) - expected.at(y, x, c));
        if (difference > largest_error) {
          largest_error = difference;
        }
      }
    }
  }
  cuda_ai_test::check(largest_error <= kTolerance,
                      label + ": max |CUDA - CPU| = " + std::to_string(largest_error) +
                          " exceeds tolerance " + std::to_string(kTolerance));
}

void test_rejects_invalid_input() {
  cuda_ai_test::HostImage source(4, 4, 1);
  cuda_ai_test::HostImage destination(4, 4, 2);
  DeviceBuffer device_source(source.storage.size());
  DeviceBuffer device_destination(destination.storage.size());

  const cuda_ai::ConstImageView source_view{device_source.get(), 4, 4, 1,
                                            source.stride_elements};
  const cuda_ai::ImageView destination_view{device_destination.get(), 4, 4, 2,
                                            destination.stride_elements};
  cuda_ai_test::check(
      cuda_ai::resize_bilinear_cuda(source_view, destination_view) ==
          cudaErrorInvalidValue,
      "channel mismatch must return cudaErrorInvalidValue without launching");

  const cuda_ai::ConstImageView null_source{nullptr, 4, 4, 1, 4};
  const cuda_ai::ImageView ok_destination{device_destination.get(), 4, 4, 1,
                                          destination.stride_elements};
  cuda_ai_test::check(
      cuda_ai::resize_bilinear_cuda(null_source, ok_destination) ==
          cudaErrorInvalidValue,
      "null source must return cudaErrorInvalidValue without launching");
}

}  // namespace

int main() {
  int device_count = 0;
  const cudaError_t device_status = cudaGetDeviceCount(&device_count);
  if (device_status != cudaSuccess || device_count == 0) {
    std::cout << "No CUDA device available ("
              << (device_status == cudaSuccess ? "count 0"
                                               : cudaGetErrorString(device_status))
              << "); skipping CUDA parity tests.\n";
    return kSkipExitCode;
  }

  try {
    for (const cuda_ai_test::ResizeCase& test_case : cuda_ai_test::resize_cases()) {
      compare_case(test_case, 0, 0);
      compare_case(test_case, 5, 3);
    }
    test_rejects_invalid_input();
  } catch (const std::exception& error) {
    std::cerr << "FAILED: " << error.what() << '\n';
    return 1;
  }

  std::cout << "All CUDA/CPU resize parity tests passed.\n";
  return 0;
}
