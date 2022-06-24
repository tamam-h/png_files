#pragma once
#include <cstdint>

enum struct pixel_type_number {
	greyscale_1,
	greyscale_2,
	greyscale_4,
	greyscale_8,
	greyscale_16,
	greyscale_float,
	truecolour_8,
	truecolour_16,
	truecolour_float,
	greyscale_with_alpha_8,
	greyscale_with_alpha_16,
	greyscale_with_alpha_float,
	truecolour_with_alpha_8,
	truecolour_with_alpha_16,
	truecolour_with_alpha_float
};

template <class> constexpr bool is_pixel_type_v{ 0 };
struct greyscale_1 {
	std::uint8_t grey;
};
template <> constexpr bool is_pixel_type_v<greyscale_1>{ 1 };
struct greyscale_2 {
	std::uint8_t grey;
};
template <> constexpr bool is_pixel_type_v<greyscale_2>{ 1 };
struct greyscale_4 {
	std::uint8_t grey;
};
template <> constexpr bool is_pixel_type_v<greyscale_4>{ 1 };
struct greyscale_8 {
	std::uint8_t grey;
};
template <> constexpr bool is_pixel_type_v<greyscale_8>{ 1 };
struct greyscale_16 {
	std::uint16_t grey;
};
template <> constexpr bool is_pixel_type_v<greyscale_16>{ 1 };
struct greyscale_float {
	float grey;
};
template <> constexpr bool is_pixel_type_v<greyscale_float>{ 1 };
struct truecolour_8 {
	std::uint8_t red, green, blue;
};
template <> constexpr bool is_pixel_type_v<truecolour_8>{ 1 };
struct truecolour_16 {
	std::uint16_t red, green, blue;
};
template <> constexpr bool is_pixel_type_v<truecolour_16>{ 1 };
struct truecolour_float {
	float red, green, blue;
};
template <> constexpr bool is_pixel_type_v<truecolour_float>{ 1 };
struct greyscale_with_alpha_8 {
	std::uint8_t grey, alpha;
};
template <> constexpr bool is_pixel_type_v<greyscale_with_alpha_8>{ 1 };
struct greyscale_with_alpha_16 {
	std::uint16_t grey, alpha;
};
template <> constexpr bool is_pixel_type_v<greyscale_with_alpha_16>{ 1 };
struct greyscale_with_alpha_float {
	float grey, alpha;
};
template <> constexpr bool is_pixel_type_v<greyscale_with_alpha_float>{ 1 };
struct truecolour_with_alpha_8 {
	std::uint8_t red, green, blue, alpha;
};
template <> constexpr bool is_pixel_type_v<truecolour_with_alpha_8>{ 1 };
struct truecolour_with_alpha_16 {
	std::uint16_t red, green, blue, alpha;
};
template <> constexpr bool is_pixel_type_v<truecolour_with_alpha_16>{ 1 };
struct truecolour_with_alpha_float {
	float red, green, blue, alpha;
};
template <> constexpr bool is_pixel_type_v<truecolour_with_alpha_float>{ 1 };

template <class type> concept pixel_type = is_pixel_type_v<type>;