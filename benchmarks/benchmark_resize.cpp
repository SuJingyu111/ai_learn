#include <algorithm>
#include <chrono>
#include <cstddef>
#include <iomanip>
#include <iostream>
#include <numeric>
#include <random>
#include <string>
#include <vector>

#include "cuda_ai/resize.h"

namespace {

struct Case {
  int source_width;
  int source_height;
  int destination_width;
  int destination_height;
  int channels;
};

double percentile(std::vector<double> values, double fraction) {
  std::sort(values.begin(), values.end());
  const std::size_t index =
      static_cast<std::size_t>(fraction * static_cast<double>(values.size() - 1));
  return values[index];
}

void run(const Case& benchmark_case, int warm_up, int iterations) {
  const std::size_t source_size =
      static_cast<std::size_t>(benchmark_case.source_width) *
      benchmark_case.source_height * benchmark_case.channels;
  const std::size_t destination_size =
      static_cast<std::size_t>(benchmark_case.destination_width) *
      benchmark_case.destination_height * benchmark_case.channels;
  std::vector<float> source(source_size);
  std::vector<float> destination(destination_size);
  std::mt19937 generator(42);
  std::uniform_real_distribution<float> distribution(0.0F, 1.0F);
  std::generate(source.begin(), source.end(), [&] { return distribution(generator); });

  const cuda_ai::ConstImageView source_view{
      source.data(), benchmark_case.source_width, benchmark_case.source_height,
      benchmark_case.channels,
      cuda_ai::packed_stride(benchmark_case.source_width, benchmark_case.channels)};
  const cuda_ai::ImageView destination_view{
      destination.data(), benchmark_case.destination_width,
      benchmark_case.destination_height, benchmark_case.channels,
      cuda_ai::packed_stride(benchmark_case.destination_width,
                             benchmark_case.channels)};

  for (int i = 0; i < warm_up; ++i) {
    cuda_ai::resize_bilinear_cpu(source_view, destination_view);
  }
  std::vector<double> milliseconds;
  milliseconds.reserve(static_cast<std::size_t>(iterations));
  for (int i = 0; i < iterations; ++i) {
    const auto start = std::chrono::steady_clock::now();
    cuda_ai::resize_bilinear_cpu(source_view, destination_view);
    const auto stop = std::chrono::steady_clock::now();
    milliseconds.push_back(
        std::chrono::duration<double, std::milli>(stop - start).count());
  }

  const double checksum = std::accumulate(destination.begin(), destination.end(), 0.0);
  std::cout << benchmark_case.source_width << 'x' << benchmark_case.source_height << ','
            << benchmark_case.destination_width << 'x'
            << benchmark_case.destination_height << ',' << benchmark_case.channels
            << ',' << std::fixed << std::setprecision(4)
            << percentile(milliseconds, 0.50) << ',' << percentile(milliseconds, 0.95)
            << ',' << checksum << '\n';
}

}  // namespace

int main(int argc, char** argv) {
  int iterations = 10;
  if (argc == 2) {
    iterations = std::max(1, std::stoi(argv[1]));
  }
  constexpr int warm_up = 3;
  const std::vector<Case> cases{{640, 360, 1280, 720, 1},
                                {1920, 1080, 1280, 720, 1},
                                {1920, 1080, 1280, 720, 4},
                                {3840, 2160, 1920, 1080, 1}};
  std::cout << "source,destination,channels,median_ms,p95_ms,checksum\n";
  for (const Case& benchmark_case : cases) {
    run(benchmark_case, warm_up, iterations);
  }
  return 0;
}
