#include <algorithm>
#include <cmath>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

#include "test_support.h"
#include "tt/resize.h"

namespace {

tt::ConstImageView const_view(const std::vector<float>& data, int width, int height,
                              int channels) {
  return {data.data(), width, height, channels, tt::packed_stride(width, channels)};
}

tt::ImageView view(std::vector<float>& data, int width, int height, int channels) {
  return {data.data(), width, height, channels, tt::packed_stride(width, channels)};
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
  tt::resize_bilinear_cpu(const_view(source, 2, 2, 1), view(destination, 2, 2, 1));
  for (std::size_t i = 0; i < source.size(); ++i) {
    expect_near(destination[i], source[i], 1e-6F, "identity");
  }
}

void test_downscale_to_average() {
  const std::vector<float> source{1.0F, 2.0F, 3.0F, 4.0F};
  std::vector<float> destination(1, 0.0F);
  tt::resize_bilinear_cpu(const_view(source, 2, 2, 1), view(destination, 1, 1, 1));
  expect_near(destination[0], 2.5F, 1e-6F, "2x2 to 1x1");
}

void test_constant_multichannel_and_odd_size() {
  constexpr int source_width = 3;
  constexpr int source_height = 5;
  constexpr int channels = 4;
  std::vector<float> source(source_width * source_height * channels, 7.25F);
  std::vector<float> destination(7 * 9 * channels, 0.0F);
  tt::resize_bilinear_cpu(const_view(source, source_width, source_height, channels),
                          view(destination, 7, 9, channels));
  for (float value : destination) {
    expect_near(value, 7.25F, 1e-6F, "constant preservation");
  }
}

void test_nearest_known_values() {
  const std::vector<float> source{1.0F, 2.0F, 3.0F, 4.0F};
  std::vector<float> destination(16, 0.0F);
  tt::resize_nearest_cpu(const_view(source, 2, 2, 1), view(destination, 4, 4, 1));
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
    tt::resize_bilinear_cpu(const_view(source, 2, 2, 1), view(destination, 2, 2, 2));
  } catch (const std::invalid_argument&) {
    threw = true;
  }
  if (!threw) {
    throw std::runtime_error("channel mismatch must throw");
  }
}

// Locks the nearest_mode documented in tt/resize.h. An exact 2x downscale
// puts every destination pixel on a .5 tie, so this fails outright if the
// rounding is ever changed to ONNX's default round_prefer_floor: that mode
// would select source columns 0,2,4,6 instead of 1,3,5,7.
void test_nearest_tie_rounds_up() {
  tt_test::HostImage source(8, 1, 1);
  for (int x = 0; x < 8; ++x) {
    source.row(0)[x] = static_cast<float>(x);
  }
  tt_test::HostImage destination(4, 1, 1);
  tt::resize_nearest_cpu(source.const_view(), destination.view());

  const float expected[4] = {1.0F, 3.0F, 5.0F, 7.0F};
  for (int x = 0; x < 4; ++x) {
    expect_near(destination.at(0, x, 0), expected[x], 0.0F,
                "nearest tie must round up (round_prefer_ceil)");
  }
}

// A padded row stride must not change a single output value, and must not be
// written through. This gives padded-stride coverage a real oracle without a
// second implementation: the packed run is the reference for the padded run.
void test_padded_stride_matches_packed() {
  for (const tt_test::ResizeCase& test_case : tt_test::resize_cases()) {
    tt_test::HostImage packed_source(test_case.source_width, test_case.source_height,
                                     test_case.channels);
    packed_source.fill_random(1234U);
    tt_test::HostImage padded_source(test_case.source_width, test_case.source_height,
                                     test_case.channels, 5);
    for (int y = 0; y < test_case.source_height; ++y) {
      for (int x = 0; x < test_case.source_width * test_case.channels; ++x) {
        padded_source.row(y)[x] = packed_source.row(y)[x];
      }
    }

    tt_test::HostImage packed_destination(
        test_case.destination_width, test_case.destination_height, test_case.channels);
    tt_test::HostImage padded_destination(test_case.destination_width,
                                          test_case.destination_height,
                                          test_case.channels, 3);
    tt::resize_bilinear_cpu(packed_source.const_view(), packed_destination.view());
    tt::resize_bilinear_cpu(padded_source.const_view(), padded_destination.view());

    const std::string label = std::string("padded stride [") + test_case.name + "]";
    tt_test::check(padded_source.padding_intact(), label + ": source padding written");
    tt_test::check(padded_destination.padding_intact(),
                   label + ": destination padding written");
    for (int y = 0; y < test_case.destination_height; ++y) {
      for (int x = 0; x < test_case.destination_width; ++x) {
        for (int c = 0; c < test_case.channels; ++c) {
          expect_near(padded_destination.at(y, x, c), packed_destination.at(y, x, c),
                      0.0F, label);
        }
      }
    }
  }
}

// Bilinear output is a convex combination of four source samples, so it can
// never leave the source range. Fixed seed keeps the failure reproducible.
void test_randomized_stays_in_source_range() {
  for (const tt_test::ResizeCase& test_case : tt_test::resize_cases()) {
    tt_test::HostImage source(test_case.source_width, test_case.source_height,
                              test_case.channels);
    source.fill_random(20260727U);
    tt_test::HostImage destination(test_case.destination_width,
                                   test_case.destination_height, test_case.channels);
    tt::resize_bilinear_cpu(source.const_view(), destination.view());

    float lowest = source.at(0, 0, 0);
    float highest = lowest;
    for (int y = 0; y < test_case.source_height; ++y) {
      for (int x = 0; x < test_case.source_width * test_case.channels; ++x) {
        lowest = std::min(lowest, source.row(y)[x]);
        highest = std::max(highest, source.row(y)[x]);
      }
    }
    for (int y = 0; y < test_case.destination_height; ++y) {
      for (int x = 0; x < test_case.destination_width * test_case.channels; ++x) {
        const float value = destination.row(y)[x];
        tt_test::check(value >= lowest - 1e-6F && value <= highest + 1e-6F,
                       std::string("randomized range [") + test_case.name +
                           "]: " + std::to_string(value) + " outside source range");
      }
    }
  }
}

// A 1x1 source has no neighbours to interpolate against, so every destination
// pixel must reproduce it exactly regardless of destination size.
void test_single_pixel_source_is_constant() {
  tt_test::HostImage source(1, 1, 3);
  source.row(0)[0] = 0.25F;
  source.row(0)[1] = 0.5F;
  source.row(0)[2] = 0.75F;
  tt_test::HostImage destination(9, 5, 3);
  tt::resize_bilinear_cpu(source.const_view(), destination.view());
  for (int y = 0; y < 5; ++y) {
    for (int x = 0; x < 9; ++x) {
      expect_near(destination.at(y, x, 0), 0.25F, 0.0F, "1x1 source channel 0");
      expect_near(destination.at(y, x, 1), 0.5F, 0.0F, "1x1 source channel 1");
      expect_near(destination.at(y, x, 2), 0.75F, 0.0F, "1x1 source channel 2");
    }
  }
}

}  // namespace

int main() {
  try {
    test_identity();
    test_downscale_to_average();
    test_constant_multichannel_and_odd_size();
    test_nearest_known_values();
    test_nearest_tie_rounds_up();
    test_invalid_input();
    test_padded_stride_matches_packed();
    test_randomized_stays_in_source_range();
    test_single_pixel_source_is_constant();
  } catch (const std::exception& error) {
    std::cerr << "FAILED: " << error.what() << '\n';
    return 1;
  }
  std::cout << "All resize CPU tests passed.\n";
  return 0;
}
