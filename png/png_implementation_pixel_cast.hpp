#pragma once
#include "png_implementation_pixel_types.hpp"

// casts one pixel type to another pixel type
template <pixel_type end_type, pixel_type start_type> end_type pixel_cast(start_type in);