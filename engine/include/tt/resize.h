#pragma once

#include "tt/image.h"

namespace tt {

// Resize contract shared by the CPU reference and the CUDA kernel, stated in
// ONNX Resize terms because docs/mastery-gates.md makes an inconsistent border
// or coordinate convention a G4 hard blocker.
//
// coordinate_transformation_mode = half_pixel
//     src = (dst + 0.5) * (src_size / dst_size) - 0.5
//
// nearest_mode = round_prefer_ceil  -- NOT the ONNX default
//     resize_nearest_cpu evaluates floor((dst + 0.5) * scale), which is
//     floor(src + 0.5): exact .5 ties round up. ONNX defaults to
//     round_prefer_floor, so results differ wherever a tie occurs. That is not
//     a corner case -- an exact 2x downscale puts every destination pixel on a
//     tie, and the two modes then disagree on every pixel. Compare against a
//     reference configured for round_prefer_ceil, or expect a full-image diff.
//
// border = clamp (edge replicate)
//     Sample coordinates outside [0, src_size - 1] clamp to the nearest border
//     pixel. The interpolation weight is deliberately computed from the
//     *unclamped* coordinate: once both taps clamp to the same pixel the lerp
//     degenerates to a no-op, so no separate out-of-range branch is needed.
//
// exclude_outside = 0, antialias = 0
//     Downscaling point-samples rather than averaging a footprint, so this is
//     not equivalent to an area/box resize.
//
// dtype/layout = float32, interleaved channels, row stride in elements
//     stride_elements may exceed a packed row; that padding is never read or
//     written. Source and destination channel counts must match.
//
// Both functions validate their inputs and throw std::invalid_argument on a
// null pointer, a non-positive dimension, mismatched channels, or a stride
// smaller than a packed row.
void resize_nearest_cpu(ConstImageView source, ImageView destination);
void resize_bilinear_cpu(ConstImageView source, ImageView destination);

}  // namespace tt
