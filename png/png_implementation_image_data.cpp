#include "pch.h"
#include "png_implementation_image_data.hpp"
#include "png_implementation_pixel_cast.hpp"
#include "png_implementation_pixel_types.hpp"

pixel_type_number image_data::get_pixel_type() const noexcept {
	return static_cast<pixel_type_number>(variant.index());
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

template <pixel_type> constexpr pixel_type_number pixel_type_number_v;

#define SPECIALIZE_PIXEL_TYPE_NUMBER_V(type)\
template <> constexpr pixel_type_number pixel_type_number_v<type> = pixel_type_number::type;
APPLY_TO_ALL_PIXEL_TYPES(SPECIALIZE_PIXEL_TYPE_NUMBER_V)

template <pixel_type type> void image_data::convert_to() {
	if (get_pixel_type() != pixel_type_number_v<type>) {
		std::vector<std::vector<type>> converted;
		std::visit( 
			[&converted](auto&& data) -> void {
			using first_held_type = typename std::decay_t<decltype(data)>::value_type;
			using held_type = typename first_held_type::value_type;
			converted.reserve(data.size());
			for (const std::vector<held_type>& i : data) {
				converted.emplace_back();
				converted.reserve(i.size());
				for (const held_type& j : i) {
					converted.back().emplace_back(pixel_cast<type, held_type>(j));
				}
			}
		}, variant);
		variant.emplace<std::vector<std::vector<type>>>(std::move(converted));
	}
}

#define INSTANTIATE_CONVERT_TO_FUNCTION(x)\
template void image_data::convert_to<x>();
APPLY_TO_ALL_PIXEL_TYPES(INSTANTIATE_CONVERT_TO_FUNCTION)

template <pixel_type type> std::vector<std::vector<type>>& image_data::get_array() {
	if (get_pixel_type() != pixel_type_number_v<type>) { convert_to<type>(); }
	return std::get<std::vector<std::vector<type>>>(variant);
}

#define INSTANTIATE_GET_ARRAY_FUNCTION(x)\
template std::vector<std::vector<x>>& image_data::get_array();
APPLY_TO_ALL_PIXEL_TYPES(INSTANTIATE_GET_ARRAY_FUNCTION)

template <pixel_type type> const std::vector<std::vector<type>>& image_data::get_array() const {
	return std::get<std::vector<std::vector<type>>>(variant);
}

#define INSTANTIATE_GET_ARRAY_CONST_FUNCTION(x)\
template const std::vector<std::vector<x>>& image_data::get_array() const;
APPLY_TO_ALL_PIXEL_TYPES(INSTANTIATE_GET_ARRAY_CONST_FUNCTION)