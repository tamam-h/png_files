#pragma once
#include <variant>
#include <vector>
#include "png_implementation_pixel_types.hpp"

struct image_data {
	// converts internal representation of image_data to a different pixel type
	template <pixel_type type> void convert_to();
	// returns a reference to std::vector<std::vector<type>>
	// automatically converts internal representation if the internal representation isn't the same as requested
	template <pixel_type type> std::vector<std::vector<type>>& get_array();
	// returns a reference to const std::vector<std::vector<type>> where type is a pixel type
	// throws if internal representation is not the same as the requested type
	template <pixel_type type> const std::vector<std::vector<type>>& get_array() const;
	// returns a pixel_type_number indicating the internal representation of image_data
	pixel_type_number get_pixel_type() const noexcept;
private:
	template <pixel_type type> using v = std::vector<std::vector<type>>;
	// order is important
	std::variant<
		v<greyscale_1>,
		v<greyscale_2>,
		v<greyscale_4>,
		v<greyscale_8>,
		v<greyscale_16>,
		v<greyscale_float>,
		v<truecolour_8>,
		v<truecolour_16>,
		v<truecolour_float>,
		v<greyscale_with_alpha_8>,
		v<greyscale_with_alpha_16>,
		v<greyscale_with_alpha_float>,
		v<truecolour_with_alpha_8>,
		v<truecolour_with_alpha_16>,
		v<truecolour_with_alpha_float>
	> variant;
};