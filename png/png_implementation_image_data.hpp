#pragma once
#include <variant>
#include <vector>
#include "png_implementation_pixel_types.hpp"

struct palette {
	void convert_to(pixel_type_number number);
	template <class pixel_type> pixel_type& operator[](unsigned int index);
	template <class pixel_type> std::vector<pixel_type>& get_array();
	template <class pixel_type> const pixel_type& operator[](unsigned int index) const;
	template <class pixel_type> const std::vector<pixel_type>& get_array() const;
	pixel_type_number get_type() const;
private:
	std::variant<
		std::monostate,
		std::vector<truecolour_8>,
		std::vector<truecolour_with_alpha_8>
	> arr;
};

struct image_data {

};