#include "pch.h"
#include "png_implementation_chunk_factory.hpp"
#include "png_implementation_chunk_types.hpp"
#include "png_implementation_image_data.hpp"
#include "png_implementation_macros.hpp"
#include "png_implementation_pixel_cast.hpp"
#include "png_implementation_pixel_types.hpp"
#include "png_implementation_zlib_stream.hpp"

#define CONVERT_NUMBER_TO_HASH_CASE(type)\
case pixel_type_number::type:\
	return pixel_type_hash::type;

pixel_type_hash to_pixel_type_hash(pixel_type_number in) {
	switch (in) {
	APPLY_TO_WRITABLE_PIXEL_TYPES(CONVERT_NUMBER_TO_HASH_CASE)
	default:
		throw std::runtime_error{ "can\'t convert from pixel_type_number to pixel_type_hash" };
	}
}

IHDR_chunk::IHDR_chunk(std::uint_fast32_t width, std::uint_fast32_t height, std::uint_fast8_t bit_depth, std::uint_fast8_t colour_type, std::uint_fast8_t compression_method, std::uint_fast8_t filter_method,
	std::uint_fast8_t interlace_method) : width{ width }, height{ height }, bit_depth{ bit_depth }, colour_type{ colour_type }, compression_method{ compression_method }, filter_method{ filter_method },
	interlace_method{ interlace_method } {}

chunk_type_t IHDR_chunk::get_type() const {
	return type;
}

std::unique_ptr<chunk_base> construct_IHDR(std::span<const std::uint8_t> in, const std::vector<std::unique_ptr<chunk_base>>& chunks) {
	if (chunks.size()) { throw std::runtime_error{ "IHDR should be the first chunk type" }; }
	const std::uint8_t* position{ in.data() };
	std::uint_fast32_t width{ read_4(position, in) }, height{ read_4(position, in) };
	std::uint_fast8_t bit_depth{ read_1(position, in) }, colour_type{ read_1(position, in) }, compression_method{ read_1(position, in) }, filter_method{ read_1(position, in) }, interlace_method{ read_1(position, in) };
	if (position != in.data() + in.size()) { throw std::runtime_error{ "IHDR chunk is longer than expected" }; }
	if (width == 0 || height == 0) { throw std::runtime_error{ "width or height of image can\'t be zero" }; }
	if (compression_method != 0) { throw std::runtime_error{ "compression method should be zero" }; }
	if (filter_method != 0) { throw std::runtime_error{ "filter method should be zero" }; }
	if (interlace_method > 1) { throw std::runtime_error{ "interlace method should be either zero or one" }; }
	if (colour_type > 6) { throw std::runtime_error{ "colour type is greater than expected" }; }
	if (bit_depth > 16) { throw std::runtime_error{ "bit depth is greater than expected" }; }
	static const std::set<pixel_type_hash> allowed_colour_type_bit_depth_pairs{ pixel_type_hash::greyscale_1, pixel_type_hash::greyscale_2, pixel_type_hash::greyscale_4, pixel_type_hash::greyscale_8, pixel_type_hash::greyscale_16,
		pixel_type_hash::truecolour_8, pixel_type_hash::truecolour_16, pixel_type_hash::indexed_colour_1, pixel_type_hash::indexed_colour_2, pixel_type_hash::indexed_colour_4, pixel_type_hash::indexed_colour_8,
		pixel_type_hash::greyscale_with_alpha_8, pixel_type_hash::greyscale_with_alpha_16, pixel_type_hash::truecolour_with_alpha_8, pixel_type_hash::truecolour_with_alpha_16 };
	if (!allowed_colour_type_bit_depth_pairs.contains(static_cast<pixel_type_hash>(colour_type << 5 | bit_depth))) {
		throw std::runtime_error{ "invalid pair of colour type and bit depth" };
	}
	return std::unique_ptr<chunk_base>{ new IHDR_chunk{ width, height, bit_depth, colour_type, compression_method, filter_method, interlace_method } };
}

#define SET_IMAGE_SIZE_CASE(type, hash)\
case hash:\
	out.get_array<type>().resize(height, std::vector<type>(width));\
	break;

void IHDR_chunk::set_image_data(image_construction_data& construction_data, image_data& out) {
	construction_data.width = width;
	construction_data.height = height;
	construction_data.bit_depth = bit_depth;
	construction_data.colour_type = colour_type;
	construction_data.uses_interlacing = interlace_method;
	switch (static_cast<pixel_type_hash>(colour_type << 5 | bit_depth)) {
	SET_IMAGE_SIZE_CASE(greyscale_1, pixel_type_hash::greyscale_1)
	SET_IMAGE_SIZE_CASE(greyscale_2, pixel_type_hash::greyscale_2)
	SET_IMAGE_SIZE_CASE(greyscale_4, pixel_type_hash::greyscale_4)
	SET_IMAGE_SIZE_CASE(greyscale_8, pixel_type_hash::greyscale_8)
	SET_IMAGE_SIZE_CASE(greyscale_16, pixel_type_hash::greyscale_16)
	SET_IMAGE_SIZE_CASE(truecolour_8, pixel_type_hash::truecolour_8)
	SET_IMAGE_SIZE_CASE(truecolour_16, pixel_type_hash::truecolour_16)
	case pixel_type_hash::indexed_colour_1:
	case pixel_type_hash::indexed_colour_2:
	case pixel_type_hash::indexed_colour_4:
	SET_IMAGE_SIZE_CASE(truecolour_8, pixel_type_hash::indexed_colour_8)
	SET_IMAGE_SIZE_CASE(greyscale_with_alpha_8, pixel_type_hash::greyscale_with_alpha_8)
	SET_IMAGE_SIZE_CASE(greyscale_with_alpha_16, pixel_type_hash::greyscale_with_alpha_16)
	SET_IMAGE_SIZE_CASE(truecolour_with_alpha_8, pixel_type_hash::truecolour_with_alpha_8)
	SET_IMAGE_SIZE_CASE(truecolour_with_alpha_16, pixel_type_hash::truecolour_with_alpha_16)
	default:
		assert(0 && "unknown pixel type");
	}
}

chunk_type_t PLTE_chunk::get_type() const {
	return type;
}

PLTE_chunk::PLTE_chunk(std::vector<truecolour_8>&& palette) : palette{ palette } {}

std::unique_ptr<chunk_base> construct_PLTE(std::span<const std::uint8_t> in, const std::vector<std::unique_ptr<chunk_base>>& chunks) {
	bool IHDR_exists{ 0 };
	for (const std::unique_ptr<chunk_base>& i : chunks) {
		assert(i->get_type() != IEND_chunk::type && "there should not be an IEND chunk in the vector of chunks");
		if (i->get_type() == IHDR_chunk::type) { IHDR_exists = 1; }
		if (i->get_type() == IDAT_chunk::type) { throw std::runtime_error{ "PLTE chunk type should come before IDAT chunks" }; }
		if (i->get_type() == PLTE_chunk::type) { throw std::runtime_error{ "expected only one PLTE chunk type" }; }
	}
	if (!IHDR_exists) { throw std::runtime_error{ "IHDR chunk should come before PLTE chunk" }; }
	if (in.size() % 3 || in.size() > 768 // 256 * 3 see https://www.w3.org/TR/2003/REC-PNG-20031110/ section 11.2.3
		) { throw std::runtime_error{ "PLTE chunk type should have a size that is a multiple of 3 and size should be less than or equal to 768" }; }
	const std::uint8_t* position{ in.data() };
	std::vector<truecolour_8> palette; palette.reserve(in.size() / 3);
	for (std::size_t i{ 0 }; i < in.size(); i += 3) {
		palette.emplace_back();
		palette.back().red = *position++;
		palette.back().green = *position++;
		palette.back().blue = *position++;
	}
	return std::unique_ptr<chunk_base>{ new PLTE_chunk{ std::move(palette) } };
}

void PLTE_chunk::set_image_data(image_construction_data& construction_data, image_data& out) {
	if (construction_data.colour_type != 3) {
		if (palette.size()) { throw std::runtime_error{ "palette should have a size of zero if the colour type is not 3" }; }
		return;
	}
	if (palette.size() > 1ull << construction_data.bit_depth) { throw std::runtime_error{ "palette size is greater than expected" }; }
	construction_data.palette = std::move(palette);
}

// https://www.w3.org/TR/2003/REC-PNG-20031110/ section 8.2
constexpr std::uint_fast8_t reduced_image_number_interlaced[8][8]{
	{ 1, 6, 4, 6, 2, 6, 4, 6 },
	{ 7, 7, 7, 7, 7, 7, 7, 7 },
	{ 5, 6, 5, 6, 5, 6, 5, 6 },
	{ 7, 7, 7, 7, 7, 7, 7, 7 },
	{ 3, 6, 4, 6, 3, 6, 4, 6 },
	{ 7, 7, 7, 7, 7, 7, 7, 7 },
	{ 5, 6, 5, 6, 5, 6, 5, 6 },
	{ 7, 7, 7, 7, 7, 7, 7, 7 }
};

dimension_struct interlaced_dimensions(std::uint_fast8_t reduced_image_number, dimension_struct in) {
	assert(in.width >= 1 && in.height >= 1 && "image size must be valid");
	assert(reduced_image_number < 7 && "there are only 7 reduced images");
	struct small_dimension_struct {
		std::int_fast8_t width, height;
	};
	static const std::vector<std::vector<std::vector<small_dimension_struct>>> dimensions{
		[]() -> std::vector<std::vector<std::vector<small_dimension_struct>>> {
			std::vector<std::vector<std::vector<small_dimension_struct>>> dimensions(7, std::vector<std::vector<small_dimension_struct>>(8, std::vector<small_dimension_struct>(8)));
			for (std::uint_fast8_t reduced_image_number{ 1 }; reduced_image_number <= 7; ++reduced_image_number) {
				for (std::uint_fast8_t max_i{ 0 }; max_i < 8; ++max_i) {
					for (std::uint_fast8_t max_j{ 0 }; max_j < 8; ++max_j) {
						small_dimension_struct temp{ -1, -1 }, acc{ 0, 0 };
						for (std::uint_fast8_t i{ 0 }; i <= max_i; ++i) {
							for (std::uint_fast8_t j{ 0 }; j <= max_j; ++j) {
								if (reduced_image_number_interlaced[i][j] == reduced_image_number && j > temp.width) {
									temp.width = j;
									++acc.width;
								}
								if (reduced_image_number_interlaced[i][j] == reduced_image_number && i > temp.height) {
									temp.height = i;
									++acc.height;
								}
							}
						}
						dimensions[reduced_image_number - 1][max_i][max_j] = acc;
					}
				}
			}
			return dimensions;
		} ()
	};
	dimension_struct first_dimension{ ((in.width - 1) >> 3) * dimensions[reduced_image_number][7][7].width, ((in.height - 1) >> 3) * dimensions[reduced_image_number][7][7].height };
	small_dimension_struct second_dimension{ dimensions[reduced_image_number][(in.height - 1) & 0b111][(in.width - 1) & 0b111] };
	if (in.height > 8) {
		second_dimension.width = dimensions[reduced_image_number][7][(in.width - 1) & 0b111].width;
	}
	if (in.width > 8) {
		second_dimension.height = dimensions[reduced_image_number][(in.height - 1) & 0b111][7].height;
	}
	dimension_struct return_value{ first_dimension.width + second_dimension.width, first_dimension.height + second_dimension.height };
	if (return_value.width == 0 || return_value.height == 0) {
		return { 0, 0 };
	}
	return return_value;
}

#define CONSTRUCT_SCANLINE_DATA_CASE(bpp, bb)\
bytes_per_pixel = bpp;\
bytes_back = bb;\
break;

scanline_data::scanline_data(const image_construction_data& construction_data, std::span<const std::uint8_t> in) : construction_data{ construction_data }, bytes_back{}, bytes_per_pixel{} {
	static const auto handle_reading{
		[](std::vector<std::vector<std::uint8_t>>& scanlines, std::uint_fast64_t width, std::uint_fast64_t height, const std::uint8_t*& position, const std::span<const std::uint8_t>& in) -> void {
			scanlines.resize(height, std::vector<std::uint8_t>(width));
			assert_can_read(position + width * height - 1, in);
			for (std::vector<std::uint8_t>& i : scanlines) {
				std::memcpy(i.data(), position, width);
				position += width;
				if (i[0] > 4) {
					// https://www.w3.org/TR/2003/REC-PNG-20031110/ section 9.2
					throw std::runtime_error{ "filter type is larger than expected" };
				}
			}
		}
	};
	// https://www.w3.org/TR/2003/REC-PNG-20031110/ section 7.2 and 9.2
	switch (static_cast<pixel_type_hash>(construction_data.colour_type << 5 | construction_data.bit_depth)) {
	case pixel_type_hash::greyscale_1:
	case pixel_type_hash::indexed_colour_1:
		CONSTRUCT_SCANLINE_DATA_CASE(0b0000'0001, 1)
	case pixel_type_hash::greyscale_2:
	case pixel_type_hash::indexed_colour_2:
		CONSTRUCT_SCANLINE_DATA_CASE(0b0000'0010, 1)
	case pixel_type_hash::greyscale_4:
	case pixel_type_hash::indexed_colour_4:
		CONSTRUCT_SCANLINE_DATA_CASE(0b0000'0100, 1)
	case pixel_type_hash::greyscale_8:
	case pixel_type_hash::indexed_colour_8:
		CONSTRUCT_SCANLINE_DATA_CASE(0b0000'1000, 1)
	case pixel_type_hash::greyscale_16:
	case pixel_type_hash::greyscale_with_alpha_8:
		CONSTRUCT_SCANLINE_DATA_CASE(0b0001'0000, 2)
	case pixel_type_hash::truecolour_8:
		CONSTRUCT_SCANLINE_DATA_CASE(0b0001'1000, 3)
	case pixel_type_hash::greyscale_with_alpha_16:
	case pixel_type_hash::truecolour_with_alpha_8:
		CONSTRUCT_SCANLINE_DATA_CASE(0b0010'0000, 4)
	case pixel_type_hash::truecolour_16:
		CONSTRUCT_SCANLINE_DATA_CASE(0b0011'0000, 6)
	case pixel_type_hash::truecolour_with_alpha_16:
		CONSTRUCT_SCANLINE_DATA_CASE(0b0100'0000, 8)
	default:
		assert(0 && "unknown pixel type");
	}
	const std::uint8_t* position{ in.data() };
	if (construction_data.uses_interlacing) {
		scanlines.resize(7);
		for (std::uint_fast8_t reduced_image_number{ 0 }; reduced_image_number < 7; ++reduced_image_number) {
			dimension_struct dimensions{ interlaced_dimensions(reduced_image_number, { construction_data.width, construction_data.height }) };
			// width is a fixed point number ddddddddd'ddddddddd'dddddddd'ddddd.ddd where d is a bit
			std::uint_fast64_t width{ static_cast<std::uint_fast64_t>(dimensions.width) * bytes_per_pixel }, height{ dimensions.height };
			// width is a fixed point numbers ddddddddd'ddddddddd'dddddddd'dddddddd where d is a bit
			width = (width >> 3) + static_cast<bool>(width & 0b0000'0111) + 1;
			handle_reading(scanlines[reduced_image_number], width, height, position, in);
		}
	} else {
		scanlines.emplace_back();
		// width is a fixed point number ddddddddd'ddddddddd'dddddddd'ddddd.ddd where d is a bit
		std::uint_fast64_t width{ static_cast<std::uint_fast64_t>(construction_data.width) * bytes_per_pixel }, height{ construction_data.height };
		// width is a fixed point numbers ddddddddd'ddddddddd'dddddddd'dddddddd where d is a bit
		width = (width >> 3) + static_cast<bool>(width & 0b0000'0111) + 1;
		assert(width > 1 && "scanlines with filter type should have a size of more than one");
		assert(height > 0 && "there should be more than one scanline");
		handle_reading(scanlines.back(), width, height, position, in);
	}
}

#define CONSTRUCT_SCANLINE_DATA_FROM_IMAGE_DATA_CASE(type)\
case pixel_type_hash::type:\
	{\
		bytes_per_pixel = integral_pixel_size<type>;\
		scanlines[0].resize(height);\
		const std::vector<std::vector<type>>& arr{ in.get_array<type>() };\
		assert(arr.size() == height && "input image does not match height given");\
		std::uint_fast64_t scanline_size{ static_cast<std::uint_fast64_t>(width) * bytes_per_pixel };\
		scanline_size = 1 + (scanline_size >> 3) + static_cast<bool>(scanline_size & 0b111);\
		if constexpr (integral_pixel_size<type> < 0b1000) {\
			assert(0b1000 % bytes_per_pixel == 0 && "if size is less than one byte per pixel, a whole number of pixels should fit in a byte");\
			for (std::uint_fast32_t i{ 0 }; i < height; ++i) {\
				scanlines[0][i].resize(scanline_size);\
				assert(arr[i].size() == width && "input image does not match width given");\
				for (std::uint_fast32_t j{ 0 }; j < width; ++j) {\
					scanlines[0][i][1ull + (static_cast<std::uint_fast64_t>(j) * bytes_per_pixel >> 3)] |= to_integral_pixel(arr[i][j]) << (8 - ((static_cast<std::uint_fast64_t>(j) * bytes_per_pixel) & 0b111) - bytes_per_pixel);\
				}\
			}\
		} else {\
			assert((bytes_per_pixel & 0b111) == 0 && "should be an integer number of bytes");\
			for (std::uint_fast32_t i{ 0 }; i < height; ++i) {\
				scanlines[0][i].resize(scanline_size);\
				assert(arr[i].size() == width && "input image does not match width given");\
				std::uint8_t* position{ scanlines[0][i].data() + 1 };\
				for (std::uint_fast32_t j{ 0 }; j < width; ++j) {\
					write(position, { bytes_per_pixel, to_integral_pixel(arr[i][j]) });\
				}\
			}\
		}\
	}\
	break;

scanline_data::scanline_data(const image_data& in, pixel_type_hash contained_type, std::uint_fast32_t width, std::uint_fast32_t height)
	: construction_data{ width, height, static_cast<std::uint_fast8_t>(static_cast<std::uint_fast64_t>(contained_type) & 0b111), static_cast<std::uint_fast8_t>((static_cast<std::uint_fast64_t>(contained_type) & 0b1111'1000) >> 3),
		0, 1, {} }, scanlines{ {} }, bytes_per_pixel{} {
	assert(contained_type != pixel_type_hash::indexed_colour_1 && contained_type != pixel_type_hash::indexed_colour_2 && contained_type != pixel_type_hash::indexed_colour_4 && contained_type != pixel_type_hash::indexed_colour_8
		&& "index pixel types are not implemented");
	assert(to_pixel_type_hash(in.get_pixel_type()) == contained_type && "passed contained_type does not match the contained type of in");
	assert(height >= 1 && height <= INT32_MAX && "height is not in range of what is expected");
	assert(width >= 1 && width <= INT32_MAX && "width is not in range of what is expected");
	bytes_back = std::max(construction_data.bit_depth, static_cast<std::uint_fast8_t>(0b1000)) >> 3;
	switch (contained_type) {
	APPLY_TO_WRITABLE_PIXEL_TYPES(CONSTRUCT_SCANLINE_DATA_FROM_IMAGE_DATA_CASE)
	default:
		assert(0 && "unknown pixel type");
	}
}

std::uint_fast64_t get_pixel(const std::vector<std::vector<std::uint8_t>>& reduced_image, std::uint_fast32_t i, std::uint_fast32_t j, std::uint_fast8_t bytes_per_pixel) {
	assert(bytes_per_pixel > 0 && "bytes_per_pixel should not be equal to zero");
	assert(i < reduced_image.size() && "invalid index i");
	const std::vector<std::uint8_t>& scanline{ reduced_image[i] };
	std::uint_fast64_t byte_index{ static_cast<std::uint_fast64_t>(bytes_per_pixel) * j }, bit_index{ byte_index & 0b111 };
	byte_index >>= 3;
	++byte_index;
	assert(byte_index < scanline.size() && "out of range");
	const std::uint8_t* position{ scanline.data() + byte_index };
	if (bytes_per_pixel < 0b1000) { return (*position & (((1 << bytes_per_pixel) - 1) << (8 - bit_index - bytes_per_pixel))) >> (8 - bit_index - bytes_per_pixel); }
	assert(bit_index == 0 && "should be at a byte boundary");
	std::uint_fast64_t acc{ 0 };
	while (bytes_per_pixel) {
		acc <<= 8;
		assert(position < scanline.data() + scanline.size() && "out of range");
		acc |= *position++;
		bytes_per_pixel -= 0b1000;
	}
	return acc;
}

#define ASSIGN_PIXEL_CASE(type, hash, use_palette)\
case hash:\
	assert(out.get_pixel_type() == pixel_type_number::type && "pixel type in out is other than expected");\
	assert(out.get_array<type>().size() == construction_data.height && "array held by out should have a height of construction_data.height");\
	{\
		std::vector<std::vector<type>>& image_array{ out.get_array<type>() };\
		if (!construction_data.uses_interlacing) {\
			assert(scanlines.size() == 1 && "there should be only one reduced image when not using interlacing");\
			for (std::uint_fast32_t i{ 0 }; i < construction_data.height; ++i) {\
				for (std::uint_fast32_t j{ 0 }; j < construction_data.width; ++j) {\
					image_array[i][j] = to_pixel<type>(get_pixel(scanlines[0], i, j, bytes_per_pixel), construction_data.palette, use_palette);\
				}\
			}\
		} else {\
			assert(scanlines.size() == 7 && "there should be seven reduced image when using interlacing");\
			for (std::uint_fast32_t i{ 0 }; i < construction_data.height; i += 8) {\
				for (std::uint_fast32_t j{ 0 }; j < construction_data.width; j += 8) {\
					for (std::uint_fast32_t delta_i{ 0 }; delta_i < 8 && i + delta_i < construction_data.height; ++delta_i) {\
						for (std::uint_fast32_t delta_j{ 0 }; delta_j < 8 && j + delta_j < construction_data.width; ++delta_j) {\
							int reduced_image_number{ reduced_image_number_interlaced[delta_i][delta_j] - 1 };\
							dimension_struct temp{ interlaced_dimensions(reduced_image_number, { j + delta_j + 1, i + delta_i + 1 }) };\
							image_array[i + delta_i][j + delta_j] = to_pixel<type>(get_pixel(scanlines[reduced_image_number], temp.height - 1, temp.width - 1, bytes_per_pixel), construction_data.palette, use_palette);\
						}\
					}\
				}\
			}\
		}\
	}\
	break;

void scanline_data::write_to(image_data& out) const {
	switch (static_cast<pixel_type_hash>(construction_data.colour_type << 5 | construction_data.bit_depth)) {
	ASSIGN_PIXEL_CASE(greyscale_1, pixel_type_hash::greyscale_1, 0)
	ASSIGN_PIXEL_CASE(greyscale_2, pixel_type_hash::greyscale_2, 0)
	ASSIGN_PIXEL_CASE(greyscale_4, pixel_type_hash::greyscale_4, 0)
	ASSIGN_PIXEL_CASE(greyscale_8, pixel_type_hash::greyscale_8, 0)
	ASSIGN_PIXEL_CASE(greyscale_16, pixel_type_hash::greyscale_16, 0)
	ASSIGN_PIXEL_CASE(truecolour_8, pixel_type_hash::truecolour_8, 0)
	ASSIGN_PIXEL_CASE(truecolour_16, pixel_type_hash::truecolour_16, 0)
	case pixel_type_hash::indexed_colour_1:
	case pixel_type_hash::indexed_colour_2:
	case pixel_type_hash::indexed_colour_4:
	ASSIGN_PIXEL_CASE(truecolour_8, pixel_type_hash::indexed_colour_8, 1)
	ASSIGN_PIXEL_CASE(greyscale_with_alpha_8, pixel_type_hash::greyscale_with_alpha_8, 0)
	ASSIGN_PIXEL_CASE(greyscale_with_alpha_16, pixel_type_hash::greyscale_with_alpha_16, 0)
	ASSIGN_PIXEL_CASE(truecolour_with_alpha_8, pixel_type_hash::truecolour_with_alpha_8, 0)
	ASSIGN_PIXEL_CASE(truecolour_with_alpha_16, pixel_type_hash::truecolour_with_alpha_16, 0)
	default:
		assert(0 && "unknown pixel type");
	}
}

void scanline_data::write_to(std::vector<std::uint8_t>& out) const {
	std::uint_fast64_t size{ 0 };
	for (const std::vector<std::vector<std::uint8_t>>& reduced_image : scanlines) {
		if (reduced_image.empty()) { continue; }
		std::uint_fast64_t scanline_size{ reduced_image[0].size() };
		assert(scanline_size > 1 && "if reduced image exists, it should not have zero width");
		assert(scanline_size <= INT32_MAX + 1ull && "size of scanline is larger than expected");
		size += scanline_size * reduced_image.size();
		for (const std::vector<std::uint8_t>& scanline : reduced_image) {
			assert(scanline.size() == scanline_size && "each scanline in a reduced image should have the same size");
		}
	}
	out.reserve(out.size() + size);
	std::uint8_t* position{ out.data() + out.size() };
	out.resize(out.size() + size);
	for (const std::vector<std::vector<std::uint8_t>>& reduced_image : scanlines) {
		if (reduced_image.empty()) { continue; }
		std::uint_fast64_t scanline_size{ reduced_image[0].size() };
		for (const std::vector<std::uint8_t>& scanline : reduced_image) {
			std::memcpy(position, scanline.data(), scanline_size);
			position += scanline_size;
		}
	}
}

void scanline_data::reconstruct_data() {
	static const auto access{
		// 64 bit signed integer because can be negative and width or height of image are 4 byte unsigned integers
		[](std::int_fast64_t i, std::int_fast64_t j, std::vector<std::vector<std::uint8_t>>& reduced_image) -> std::uint8_t {
			return (i < 0 || j < 1) ? 0 : reduced_image[i][j];
		}
	};
	static const auto paeth{
		// https://www.w3.org/TR/2003/REC-PNG-20031110/ section 9.4
		[](std::uint_fast8_t a, std::uint_fast8_t b, std::uint_fast8_t c) -> std::uint_fast8_t {
			int p{ a + b - c };
			int pa{ std::abs(p - a) }, pb{ std::abs(p - b) }, pc{ std::abs(p - c) };
			if (pa <= pb && pa <= pc) { return a; }
			if (pb <= pc) { return b; }
			return c;
		}
	};
	for (std::vector<std::vector<std::uint8_t>>& reduced_image : scanlines) {
		for (std::int_fast64_t i{ 0 }; static_cast<std::size_t>(i) < reduced_image.size(); ++i) {
			assert(reduced_image[i].size() > 1 && "scanlines should have a size of two or more: filter type and pixel data");
			for (std::int_fast64_t j{ 1 }; static_cast<std::size_t>(j) < reduced_image[i].size(); ++j) {
				std::uint_fast8_t a{ access(i, j - bytes_back, reduced_image) }, b{ access(i - 1, j, reduced_image) };
				switch (reduced_image[i][0]) {
				case 0:
					break;
				case 1:
					reduced_image[i][j] += a;
					break;
				case 2:
					reduced_image[i][j] += b;
					break;
				case 3:
					reduced_image[i][j] += (a + b) >> 1;
					break;
				case 4:
					reduced_image[i][j] += paeth(a, b, access(i - 1, j - bytes_back, reduced_image));
					break;
				default:
					assert(0 && "larger filter type than expected");
				}
				reduced_image[i][0] = 0;
			}
		}
	}
}

std::uint_fast8_t scanline_data::filter_data() {
	assert(scanlines.size() == 1 && "unimplemented when there is more than one reduced image");
	assert(scanlines[0].size() && "height of the image should be greater than or equal to one");
	assert(scanlines[0].size() <= INT32_MAX + 1ull && "height of the image is larger than expected");
	std::uint_fast64_t scanline_size{ scanlines[0][0].size() };
	assert(scanline_size > 1 && "scanline size is smaller than expected");
	assert(scanline_size <= INT32_MAX + 1ull && "scanline size is greater than expected");
	for (std::vector<std::uint8_t>& i : scanlines[0]) {
		assert(i.size() == scanline_size && "each scanline should be the same size in a reduced image");
		i[0] = 0;
	}
	assert(!construction_data.uses_interlacing && "unimplemented when uses interlacing");
	return 0;
}

chunk_type_t IDAT_chunk::get_type() const {
	return type;
}

IDAT_chunk::IDAT_chunk(std::shared_ptr<std::vector<std::uint8_t>> ptr) : zlib_stream{ ptr } {}

std::unique_ptr<chunk_base> construct_IDAT(std::span<const std::uint8_t> in, const std::vector<std::unique_ptr<chunk_base>>& chunks) {
	bool IHDR_exists{ 0 }, IDAT_exists{ 0 };
	for (const std::unique_ptr<chunk_base>& i : chunks) {
		assert(i->get_type() != IEND_chunk::type && "there should not be an IEND chunk in the vector of chunks");
		if (i->get_type() == IHDR_chunk::type) {
			IHDR_exists = 1;
		}
		if (i->get_type() == IDAT_chunk::type) {
			IDAT_exists = 1;
		}
	}
	if (!IHDR_exists) { throw std::runtime_error{ "IDAT chunk type should come after IHDR chunk type" }; }
	if (IDAT_exists && chunks.back()->get_type() != IDAT_chunk::type) { throw std::runtime_error{ "IDAT chunks must be adjacent" }; }
	std::shared_ptr<std::vector<std::uint8_t>> vector{ IDAT_exists ? static_cast<IDAT_chunk*>(chunks.back().get())->zlib_stream
		: std::shared_ptr<std::vector<std::uint8_t>>{ new std::vector<std::uint8_t> } };
	vector->reserve(vector->size() + in.size());
	std::uint8_t* destination{ vector->data() + vector->size() };
	vector->resize(vector->size() + in.size());
	std::memcpy(destination, in.data(), in.size());
	return std::unique_ptr<chunk_base>{ new IDAT_chunk{ vector } };
}

void IDAT_chunk::set_image_data(image_construction_data& construction_data, image_data& out) {
	if (!construction_data.pixel_data_is_set) {
		zlib_stream_handler stream;
		stream.compressed_data = std::move(*zlib_stream);
		stream.decompress();
		scanline_data scanlines{ construction_data, { stream.uncompressed_data.data(), stream.uncompressed_data.data() + stream.uncompressed_data.size() } };
		scanlines.reconstruct_data();
		scanlines.write_to(out);
		construction_data.pixel_data_is_set = 1;
	}
}

chunk_type_t IEND_chunk::get_type() const {
	return type;
}

std::unique_ptr<chunk_base> construct_IEND(std::span<const std::uint8_t> in, const std::vector<std::unique_ptr<chunk_base>>& chunks) {
	if (in.size()) { throw std::runtime_error{ "IEND chunk type is expected to have zero size" }; }
	bool IDAT_exists{ 0 };
	for (const std::unique_ptr<chunk_base>& i : chunks) {
		if (i->get_type() == IDAT_chunk::type) { IDAT_exists = 1; }
	}
	if (!IDAT_exists) { throw std::runtime_error{ "IDAT chunk type should come before IEND chunk type" }; }
	return std::unique_ptr<chunk_base>{ new IEND_chunk };
}

void IEND_chunk::set_image_data(image_construction_data& construction_data, image_data& out) {}

void register_chunk_types() {
	register_chunk_type(IHDR_chunk::type, construct_IHDR);
	register_chunk_type(PLTE_chunk::type, construct_PLTE);
	register_chunk_type(IDAT_chunk::type, construct_IDAT);
	register_chunk_type(IEND_chunk::type, construct_IEND);
}