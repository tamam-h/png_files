#pragma once
#include <memory>
#include "png_implementation_pixel_types.hpp"
#include <span>
#include <vector>

struct png {
	// returns a reference to std::vector<std::vector<type>>
	// automatically converts internal representation if the internal representation isn't the same as requested
	template <pixel_type type> std::vector<std::vector<type>>& get_array();
	// returns a reference to const std::vector<std::vector<type>> where type is a pixel type
	// throws if internal representation is not the same as the requested type
	template <pixel_type type> const std::vector<std::vector<type>>& get_array() const;
	// converts internal representation of image_data to a different pixel type
	template <pixel_type type> void convert_to();
	// converts internal representation of image_data to a different pixel type
	void convert_to(pixel_type_number in);
	// returns a pixel_type_number indicating the internal representation of image_data
	pixel_type_number get_pixel_type() const noexcept;
	png(std::span<const std::uint8_t> in);
	png();
	~png();
private:
	struct implementation;
	std::unique_ptr<implementation> pointer_to_implementation;
};