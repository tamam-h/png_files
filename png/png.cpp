#include "pch.h"
#include "png.hpp"
#include "png_implementation_chunk_factory.hpp"
#include "png_implementation_image_data.hpp"

struct png::implementation : image_data {};

template <pixel_type type> std::vector<std::vector<type>>& png::get_array() {
	return pointer_to_implementation->get_array<type>();
}

template <pixel_type type> const std::vector<std::vector<type>>& png::get_array() const {
	return pointer_to_implementation->get_array<type>();
}

template <pixel_type type> void png::convert_to() {
	pointer_to_implementation->convert_to<type>();
}

#define APPLY_TO_ALL_PIXEL_TYPES(macro)\
macro(greyscale_1)\
macro(greyscale_2)\
macro(greyscale_4)\
macro(greyscale_8)\
macro(greyscale_16)\
macro(greyscale_float)\
macro(truecolour_8)\
macro(truecolour_16)\
macro(truecolour_float)\
macro(greyscale_with_alpha_8)\
macro(greyscale_with_alpha_16)\
macro(greyscale_with_alpha_float)\
macro(truecolour_with_alpha_8)\
macro(truecolour_with_alpha_16)\
macro(truecolour_with_alpha_float)

#define INSTANTIATE_FUNCTIONS(type)\
template std::vector<std::vector<type>>& png::get_array<type>();\
template const std::vector<std::vector<type>>& png::get_array<type>() const;\
template void png::convert_to<type>();

APPLY_TO_ALL_PIXEL_TYPES(INSTANTIATE_FUNCTIONS)

#define CONVERT_TO_CASE(type)\
case pixel_type_number::type:\
	pointer_to_implementation->convert_to<type>();\
	break;

void png::convert_to(pixel_type_number in) {
	switch (in) {
	APPLY_TO_ALL_PIXEL_TYPES(CONVERT_TO_CASE)
	}
}

pixel_type_number png::get_pixel_type() const noexcept {
	return pointer_to_implementation->get_pixel_type();
}

png::png(std::span<const std::uint8_t> in) : pointer_to_implementation{ new implementation } {
	std::vector<std::unique_ptr<chunk_base>> chunks;
	create_chunks(chunks, in);
	set_image_data(*pointer_to_implementation, chunks);
}

png::png() : pointer_to_implementation{ new implementation } {}

png::~png() {}