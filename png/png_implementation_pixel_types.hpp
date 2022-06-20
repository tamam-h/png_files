#pragma once
#include <cstdint>

// order is important
// https://www.w3.org/TR/2003/REC-PNG-20031110/ section 11.2.2
enum struct pixel_types {
	greyscale_1, greyscale_2, greyscale_4, greyscale_8,
	truecolour_8, truecolour_16,
	indexed_colour_1, indexed_colour_2, indexed_colour_4, indexed_colour_8,
	greyscale_with_alpha_8, greyscale_with_alpha_16,
	truecolour_with_alpha_8, truecolour_with_alpha_16
};

struct greyscale_1 {
	std::uint8_t grey;
};
struct greyscale_2 {
	std::uint8_t grey;
};
struct greyscale_4 {
	std::uint8_t grey;
};
struct greyscale_8 {
	std::uint8_t grey;
};
struct greyscale_16 {
	std::uint16_t grey;
};
struct truecolour_8 {
	std::uint8_t red, green, blue;
};
struct truecolour_16 {
	std::uint16_t red, green, blue;
};
struct indexed_colour_1 {
	std::uint8_t index;
};
struct indexed_colour_2 {
	std::uint8_t index;
};
struct indexed_colour_4 {
	std::uint8_t index;
};
struct indexed_colour_8 {
	std::uint8_t index;
};
struct greyscale_with_alpha_8 {
	std::uint8_t grey, alpha;
};
struct greyscale_with_alpha_16 {
	std::uint16_t grey, alpha;
};
struct truecolour_with_alpha_8 {
	std::uint8_t red, green, blue, alpha;
};
struct truecolour_with_alpha_16 {
	std::uint16_t red, green, blue, alpha;
};