#pragma once
#include <cstdint>
#include <memory>
#include "png_implementation_chunk_factory.hpp"
#include "png_implementation_image_data.hpp"
#include "png_implementation_pixel_types.hpp"
#include <vector>

std::unique_ptr<chunk_base> construct_IHDR(std::span<const std::uint8_t> in, const std::vector<std::unique_ptr<chunk_base>>& chunks);
// https://www.w3.org/TR/2003/REC-PNG-20031110/ section 11.2.2
struct IHDR_chunk : chunk_base {
	static constexpr chunk_type_t type{ 0x4948'4452 };
	chunk_type_t get_type() const override;
	void set_image_data(image_construction_data& construction_data, image_data& out) override;
	std::uint_fast32_t width, height;
	std::uint_fast8_t bit_depth, colour_type, compression_method, filter_method, interlace_method;
	IHDR_chunk(std::uint_fast32_t width, std::uint_fast32_t height, std::uint_fast8_t bit_depth, std::uint_fast8_t colour_type, std::uint_fast8_t compression_method, std::uint_fast8_t filter_method, std::uint_fast8_t interlace_method);
};

std::unique_ptr<chunk_base> construct_PLTE(std::span<const std::uint8_t> in, const std::vector<std::unique_ptr<chunk_base>>& chunks);
// https://www.w3.org/TR/2003/REC-PNG-20031110/ section 11.2.3
struct PLTE_chunk : chunk_base {
	static constexpr chunk_type_t type{ 0x504c'5445 };
	chunk_type_t get_type() const override;
	void set_image_data(image_construction_data& construction_data, image_data& out) override;
	std::vector<truecolour_8> palette;
	PLTE_chunk(std::vector<truecolour_8>&& palette);
};

std::unique_ptr<chunk_base> construct_IDAT(std::span<const std::uint8_t> in, const std::vector<std::unique_ptr<chunk_base>>& chunks);
// https://www.w3.org/TR/2003/REC-PNG-20031110/ section 11.2.4
struct IDAT_chunk : chunk_base {
	static constexpr chunk_type_t type{ 0x4944'4154 };
	chunk_type_t get_type() const override;
	void set_image_data(image_construction_data& construction_data, image_data& out) override;
	std::shared_ptr<std::vector<std::uint8_t>> zlib_stream;
	IDAT_chunk(std::shared_ptr<std::vector<std::uint8_t>> ptr);
};

std::unique_ptr<chunk_base> construct_IEND(std::span<const std::uint8_t> in, const std::vector<std::unique_ptr<chunk_base>>& chunks);
// https://www.w3.org/TR/2003/REC-PNG-20031110/ section 11.2.5
struct IEND_chunk : chunk_base {
	static constexpr chunk_type_t type{ end_chunk_type };
	chunk_type_t get_type() const override;
	void set_image_data(image_construction_data& construction_data, image_data& out) override;
};

// registers chunk types listed in this header file
void register_chunk_types();

// https://www.w3.org/TR/2003/REC-PNG-20031110/ section 11.2.2
// colour_type << 5 | bit_depth
enum struct pixel_type_hash {
	greyscale_1 = 0b0000'0001,
	greyscale_2 = 0b0000'0010,
	greyscale_4 = 0b0000'0100,
	greyscale_8 = 0b0000'1000,
	greyscale_16 = 0b0001'0000,
	truecolour_8 = 0b0100'1000,
	truecolour_16 = 0b0101'0000,
	indexed_colour_1 = 0b0110'0001,
	indexed_colour_2 = 0b0110'0010,
	indexed_colour_4 = 0b0110'0100,
	indexed_colour_8 = 0b0110'1000,
	greyscale_with_alpha_8 = 0b1000'1000,
	greyscale_with_alpha_16 = 0b1001'0000,
	truecolour_with_alpha_8 = 0b1100'1000,
	truecolour_with_alpha_16 = 0b1101'0000
};

struct scanline_data {
	// reads in and produces scanlines
	// construction_data should exist for the lifetime of scanline_data
	scanline_data(const image_construction_data& construction_data, std::span<const std::uint8_t> in);
	// writes bytes as type to out
	void write_to(image_data& out);
	// assumes all filter methods are less than or equal to 4
	void reconstruct_data();
private:
	const image_construction_data& construction_data;
	// bytes per pixel is a fixed point number ddddd.ddd where d is a bit
	std::uint_fast8_t bytes_back, bytes_per_pixel;
	// first index is reduced image number
	// second index is scanline number
	// scanlines[ ][ ][0] is filter method
	std::vector<std::vector<std::vector<std::uint8_t>>> scanlines;
};

struct dimension_struct {
	std::uint_fast32_t width, height;
};

// in.width is assumed to be greater than 0
// in.height is assumed to be greater than 0
// reduced_image_number is assumed to be less than 7
dimension_struct interlaced_dimensions(std::uint_fast8_t reduced_image_number, dimension_struct in);

// gets a pixel from reduced image
// first byte in each scanline is filter type
// reduced_image[ ] returns a scanline
// bytes per pixel is a fixed point number ddddd.ddd where d is a bit
// i is first index
// j is second index
// least significant byte is last byte read from scanline in reduced image
// bytes per pixel is assumed to be 0.125, 0.25, 0.5, 1, 2, 3, 4, 6, or 8
// https://www.w3.org/TR/2003/REC-PNG-20031110/ section 7.2
std::uint_fast64_t get_pixel(const std::vector<std::vector<std::uint8_t>>& reduced_image, std::uint_fast32_t i, std::uint_fast32_t j, std::uint_fast8_t bytes_per_pixel);