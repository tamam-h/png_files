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
// is automatically called at beginning of program
void register_chunk_types();