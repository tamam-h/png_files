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

struct integral_pixel_info {
	// bytes is a fixed point number ddddd.ddd where d is a bit
	std::uint_fast8_t bytes;
	// value is the pixel where the most significant byte is the first byte to be written
	// the size of value is specified by bytes
	std::uint_fast64_t value;
};

// converts a pixel to std::uint_fast64_t
// most significant byte should be written first
// to be used in conjunction with integral_pixel_size
// assumes type is not a floating point pixel type
// assumes elements in in are under 1 << (bit depth)
template <pixel_type type> std::uint_fast64_t to_integral_pixel(type in);

// integral_pixel_size is a fixed point number ddddd.ddd where d is a bit
// to be used in conjunction with to_integral_pixel to make integral_pixel_info
template <pixel_type type> constexpr std::uint_fast8_t integral_pixel_size{ UINT_FAST8_MAX };
template <> constexpr std::uint_fast8_t integral_pixel_size<greyscale_1>{ 0b1 };
template <> constexpr std::uint_fast8_t integral_pixel_size<greyscale_2>{ 0b10 };
template <> constexpr std::uint_fast8_t integral_pixel_size<greyscale_4>{ 0b100 };
template <> constexpr std::uint_fast8_t integral_pixel_size<greyscale_8>{ 0b1000 };
template <> constexpr std::uint_fast8_t integral_pixel_size<greyscale_16>{ 0b1'0000 };
template <> constexpr std::uint_fast8_t integral_pixel_size<truecolour_8>{ 0b1'1000 };
template <> constexpr std::uint_fast8_t integral_pixel_size<truecolour_16>{ 0b11'0000 };
template <> constexpr std::uint_fast8_t integral_pixel_size<greyscale_with_alpha_8>{ 0b1'0000 };
template <> constexpr std::uint_fast8_t integral_pixel_size<greyscale_with_alpha_16>{ 0b10'0000 };
template <> constexpr std::uint_fast8_t integral_pixel_size<truecolour_with_alpha_8>{ 0b10'0000 };
template <> constexpr std::uint_fast8_t integral_pixel_size<truecolour_with_alpha_16>{ 0b100'0000 };

// writes an integral pixel to position and then advances position the number of bytes written
// assumes in.bytes is greater than or equal to 0b1000
// assumes in.bytes & 0b111 is equal to zero
void write(std::uint8_t*& position, integral_pixel_info in);