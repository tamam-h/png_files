#pragma once
#include <cstdint>
#include "png_implementation_pixel_types.hpp"
#include <vector>

// casts one pixel type to another pixel type
template <pixel_type end_type, pixel_type start_type> end_type pixel_cast(start_type in);

// least significant byte of in is last byte read from scanline in reduced image
// if palette is used, throws if in is greater than or equal to palette.size()
// assumes in is less than the 1 << (the size of type in bytes times 8)
// https://www.w3.org/TR/2003/REC-PNG-20031110/ section 7.2
// assumes type is truecolour_8 if uses_palette
template <pixel_type type> type to_pixel(std::uint_fast64_t in, const std::vector<truecolour_8>& palette, bool uses_palette);