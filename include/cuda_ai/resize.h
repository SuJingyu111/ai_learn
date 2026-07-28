#pragma once

#include "cuda_ai/image.h"

namespace cuda_ai {

// Pixel-center mapping: src = (dst + 0.5) * (src_size / dst_size) - 0.5.
// Out-of-range sample coordinates are clamped to the nearest border pixel.
void resize_nearest_cpu(ConstImageView source, ImageView destination);
void resize_bilinear_cpu(ConstImageView source, ImageView destination);

}  // namespace cuda_ai
