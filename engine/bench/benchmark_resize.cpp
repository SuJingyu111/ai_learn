#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstddef>
#include <cstdlib>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <numeric>
#include <random>
#include <string>
#include <vector>

#include "tt/resize.h"

#ifndef TT_BUILD_TYPE
#define TT_BUILD_TYPE "unknown"
#endif
#ifndef TT_COMPILER
#define TT_COMPILER "unknown"
#endif
#ifndef TT_FLAGS
#define TT_FLAGS "unknown"
#endif

namespace {

// A p95 estimate needs enough samples that the 95th percentile is not simply
// the largest or the smallest observation. Below this count the column is
// reported as NA rather than printing a number that cannot be defended.
constexpr int kMinimumSamplesForP95 = 20;
constexpr int kWarmUp = 3;

struct Case {
  int source_width;
  int source_height;
  int destination_width;
  int destination_height;
  int channels;
};

// Nearest-rank percentile: rank = ceil(fraction * n), index = rank - 1.
// The previous fraction * (n - 1) form collapsed to index 0 for small n, so
// median and p95 both reported the fastest run.
double percentile(const std::vector<double>& sorted_values, double fraction) {
  const std::size_t count = sorted_values.size();
  const double rank = std::ceil(fraction * static_cast<double>(count));
  std::size_t index = rank < 1.0 ? 0 : static_cast<std::size_t>(rank) - 1;
  if (index >= count) {
    index = count - 1;
  }
  return sorted_values[index];
}

std::string utc_timestamp() {
  const std::time_t now = std::time(nullptr);
  std::tm utc{};
#if defined(_WIN32)
  gmtime_s(&utc, &now);
#else
  gmtime_r(&now, &utc);
#endif
  char buffer[32];
  std::strftime(buffer, sizeof(buffer), "%Y-%m-%dT%H:%M:%SZ", &utc);
  return buffer;
}

std::string environment_or(const char* name, const char* fallback) {
  const char* value = std::getenv(name);
  return (value != nullptr && *value != '\0') ? value : fallback;
}

// docs/benchmark-methodology.md requires date, commit, host, build type,
// compiler, warm-up count, iteration count and the timed region on every run.
// The commit comes from the environment rather than a CMake configure-time
// define: a define goes stale as soon as the source is rebuilt without
// reconfiguring, which would put a wrong sha next to real numbers.
void print_provenance(int iterations) {
  std::cout << "# utc=" << utc_timestamp()
            << " commit=" << environment_or("TT_COMMIT", "unversioned")
            << " host=" << environment_or("TT_HOST", "unrecorded") << '\n'
            << "# cpu=" << environment_or("TT_CPU", "unrecorded") << '\n'
            << "# build_type=" << TT_BUILD_TYPE << " compiler=" << TT_COMPILER
            << " timer=steady_clock\n"
            << "# flags=" << TT_FLAGS << '\n'
            // A resize timing is meaningless without the semantics it ran, and
            // this contract is what docs/architecture.md and tt/resize.h pin down.
            << "# op=resize_bilinear coord=half_pixel border=clamp"
            << " dtype=f32 layout=interleaved\n"
            << "# warmup=" << kWarmUp << " iterations=" << iterations
            << " timed_region=resize_bilinear_cpu-only"
            << " validation=outside-timed-region\n";
  if (iterations < kMinimumSamplesForP95) {
    std::cout << "# note: iterations < " << kMinimumSamplesForP95
              << ", p95_ms reported as NA\n";
  }
}

void run(const Case& benchmark_case, int iterations) {
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

  const tt::ConstImageView source_view{
      source.data(), benchmark_case.source_width, benchmark_case.source_height,
      benchmark_case.channels,
      tt::packed_stride(benchmark_case.source_width, benchmark_case.channels)};
  const tt::ImageView destination_view{
      destination.data(), benchmark_case.destination_width,
      benchmark_case.destination_height, benchmark_case.channels,
      tt::packed_stride(benchmark_case.destination_width, benchmark_case.channels)};

  for (int i = 0; i < kWarmUp; ++i) {
    tt::resize_bilinear_cpu(source_view, destination_view);
  }
  std::vector<double> milliseconds;
  milliseconds.reserve(static_cast<std::size_t>(iterations));
  for (int i = 0; i < iterations; ++i) {
    const auto start = std::chrono::steady_clock::now();
    tt::resize_bilinear_cpu(source_view, destination_view);
    const auto stop = std::chrono::steady_clock::now();
    milliseconds.push_back(
        std::chrono::duration<double, std::milli>(stop - start).count());
  }
  std::sort(milliseconds.begin(), milliseconds.end());

  const double checksum = std::accumulate(destination.begin(), destination.end(), 0.0);
  std::cout << benchmark_case.source_width << 'x' << benchmark_case.source_height << ','
            << benchmark_case.destination_width << 'x'
            << benchmark_case.destination_height << ',' << benchmark_case.channels
            << ',' << iterations << ',' << std::fixed << std::setprecision(4)
            << percentile(milliseconds, 0.50) << ',';
  if (iterations >= kMinimumSamplesForP95) {
    std::cout << percentile(milliseconds, 0.95);
  } else {
    std::cout << "NA";
  }
  std::cout << ',' << milliseconds.front() << ',' << checksum << '\n';
}

}  // namespace

int main(int argc, char** argv) {
  int iterations = 100;
  if (argc == 2) {
    iterations = std::max(1, std::stoi(argv[1]));
  }
  if (iterations < kMinimumSamplesForP95) {
    std::cerr << "warning: " << iterations << " iterations is below the "
              << kMinimumSamplesForP95
              << " needed for a p95 estimate; p95_ms will be NA. Use this run "
                 "as a smoke check, not as performance evidence.\n";
  }

  const std::vector<Case> cases{{640, 360, 1280, 720, 1},
                                {1920, 1080, 1280, 720, 1},
                                {1920, 1080, 1280, 720, 4},
                                {3840, 2160, 1920, 1080, 1}};
  print_provenance(iterations);
  std::cout << "source,destination,channels,n,median_ms,p95_ms,min_ms,checksum\n";
  for (const Case& benchmark_case : cases) {
    run(benchmark_case, iterations);
  }
  return 0;
}
