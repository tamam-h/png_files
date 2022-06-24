#include "pch.h"
#include "png_implementation_deflate_algorithm.hpp"
#include "png_implementation_chunk_factory.hpp"
#include "png_implementation_chunk_types.hpp"
#include "png_implementation_image_data.hpp"
#include "png_implementation_pixel_types.hpp"

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

void IHDR_chunk::set_image_data(image_construction_data& construction_data, image_data& out) {
	construction_data.width = width;
	construction_data.height = height;
	construction_data.bit_depth = bit_depth;
	construction_data.colour_type = colour_type;
	construction_data.uses_interlacing = interlace_method;
	switch (colour_type << 5 | bit_depth) {
	case static_cast<int>(pixel_type_hash::greyscale_1):
		out.convert_to<greyscale_1>();
		break;
	case static_cast<int>(pixel_type_hash::greyscale_2):
		out.convert_to<greyscale_2>();
		break;
	case static_cast<int>(pixel_type_hash::greyscale_4):
		out.convert_to<greyscale_4>();
		break;
	case static_cast<int>(pixel_type_hash::greyscale_8):
		out.convert_to<greyscale_8>();
		break;
	case static_cast<int>(pixel_type_hash::greyscale_16):
		out.convert_to<greyscale_16>();
		break;
	case static_cast<int>(pixel_type_hash::truecolour_8):
		out.convert_to<truecolour_8>();
		break;
	case static_cast<int>(pixel_type_hash::truecolour_16):
		out.convert_to<truecolour_16>();
		break;
	case static_cast<int>(pixel_type_hash::indexed_colour_1):
	case static_cast<int>(pixel_type_hash::indexed_colour_2):
	case static_cast<int>(pixel_type_hash::indexed_colour_4):
	case static_cast<int>(pixel_type_hash::indexed_colour_8):
		out.convert_to<truecolour_8>();
		break;
	case static_cast<int>(pixel_type_hash::greyscale_with_alpha_8):
		out.convert_to<greyscale_with_alpha_8>();
		break;
	case static_cast<int>(pixel_type_hash::greyscale_with_alpha_16):
		out.convert_to<greyscale_with_alpha_16>();
		break;
	case static_cast<int>(pixel_type_hash::truecolour_with_alpha_8):
		out.convert_to<greyscale_with_alpha_8>();
		break;
	case static_cast<int>(pixel_type_hash::truecolour_with_alpha_16):
		out.convert_to<greyscale_with_alpha_16>();
		break;
	default:
		assert(0);
	}
}

chunk_type_t PLTE_chunk::get_type() const {
	return type;
}

PLTE_chunk::PLTE_chunk(std::vector<truecolour_8>&& palette) : palette{ palette } {}

std::unique_ptr<chunk_base> construct_PLTE(std::span<const std::uint8_t> in, const std::vector<std::unique_ptr<chunk_base>>& chunks) {
	bool IHDR_exists{ 0 };
	for (const std::unique_ptr<chunk_base>& i : chunks) {
		assert(i->get_type() != IEND_chunk::type);
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

scanline_data::scanline_data(const image_construction_data& construction_data, std::span<const std::uint8_t> in) : construction_data{ construction_data }, bytes_back{}, bytes_per_pixel{} {
	// https://www.w3.org/TR/2003/REC-PNG-20031110/ section 7.2 and 9.2
	switch (construction_data.colour_type << 5 | construction_data.bit_depth) {
	case static_cast<int>(pixel_type_hash::greyscale_1):
	case static_cast<int>(pixel_type_hash::indexed_colour_1):
		bytes_per_pixel = 0b0000'0001;
		bytes_back = 1;
		break;
	case static_cast<int>(pixel_type_hash::greyscale_2):
	case static_cast<int>(pixel_type_hash::indexed_colour_2):
		bytes_per_pixel = 0b0000'0010;
		bytes_back = 1;
		break;
	case static_cast<int>(pixel_type_hash::greyscale_4):
	case static_cast<int>(pixel_type_hash::indexed_colour_4):
		bytes_per_pixel = 0b0000'0100;
		bytes_back = 1;
		break;
	case static_cast<int>(pixel_type_hash::greyscale_8):
	case static_cast<int>(pixel_type_hash::indexed_colour_8):
		bytes_per_pixel = 0b0000'1000;
		bytes_back = 1;
		break;
	case static_cast<int>(pixel_type_hash::greyscale_16):
	case static_cast<int>(pixel_type_hash::greyscale_with_alpha_8):
		bytes_per_pixel = 0b0001'0000;
		bytes_back = 2;
		break;
	case static_cast<int>(pixel_type_hash::truecolour_8):
		bytes_per_pixel = 0b0001'1000;
		bytes_back = 3;
		break;
	case static_cast<int>(pixel_type_hash::greyscale_with_alpha_16):
	case static_cast<int>(pixel_type_hash::truecolour_with_alpha_8):
		bytes_per_pixel = 0b0010'0000;
		bytes_back = 4;
		break;
	case static_cast<int>(pixel_type_hash::truecolour_16):
		bytes_per_pixel = 0b0011'0000;
		bytes_back = 6;
		break;
	case static_cast<int>(pixel_type_hash::truecolour_with_alpha_16):
		bytes_per_pixel = 0b0100'0000;
		bytes_back = 8;
		break;
	default:
		assert(0);
	}
	if (construction_data.uses_interlacing) {
		
	} else {
		scanlines.emplace_back();
		// width is a fixed point number ddddddddd'ddddddddd'dddddddd'ddddd.ddd where d is a bit
		std::uint_fast64_t width{ static_cast<std::uint_fast64_t>(construction_data.width) * bytes_per_pixel }, height{ static_cast<std::uint_fast64_t>(construction_data.height) };
		// width is a fixed point numbers ddddddddd'ddddddddd'dddddddd'dddddddd where d is a bit
		width = (width >> 3) + static_cast<bool>(width & 0b0000'0111) + 1;
		assert(width > 1);
		assert(height > 0);
		scanlines.back().resize(height, std::vector<std::uint8_t>(width));
		const std::uint8_t* position{ in.data() };
		assert_can_read(position + width * height - 1, in);
		for (std::vector<std::uint8_t>& i : scanlines.back()) {
			std::memcpy(i.data(), position, width);
			position += width;
			if (i[0] > 4) {
				// https://www.w3.org/TR/2003/REC-PNG-20031110/ section 9.2
				throw std::runtime_error{ "filter method is larger than expected" };
			}
		}
	}
}

void scanline_data::write_to(image_data& out) {}

void scanline_data::reconstruct_data() {}

chunk_type_t IDAT_chunk::get_type() const {
	return type;
}

IDAT_chunk::IDAT_chunk(std::shared_ptr<std::vector<std::uint8_t>> ptr) : zlib_stream{ ptr } {}

std::unique_ptr<chunk_base> construct_IDAT(std::span<const std::uint8_t> in, const std::vector<std::unique_ptr<chunk_base>>& chunks) {
	bool IHDR_exists{ 0 };
	for (const std::unique_ptr<chunk_base>& i : chunks) {
		assert(i->get_type() != IEND_chunk::type);
		if (i->get_type() == IHDR_chunk::type) {
			IHDR_exists = 1;
			break;
		}
	}
	if (!IHDR_exists) { throw std::runtime_error{ "IDAT chunk type should come after IHDR chunk type" }; }
	std::shared_ptr<std::vector<std::uint8_t>> vector{ chunks.back()->get_type() == IDAT_chunk::type ? static_cast<IDAT_chunk*>(chunks.back().get())->zlib_stream
		: std::shared_ptr<std::vector<std::uint8_t>>{ new std::vector<std::uint8_t> } };
	vector->reserve(vector->size() + in.size());
	std::uint8_t* destination{ vector->data() + vector->size() };
	vector->resize(vector->size() + in.size());
	std::memcpy(destination, in.data(), in.size());
	return std::unique_ptr<chunk_base>{ new IDAT_chunk{ vector } };
}

void IDAT_chunk::set_image_data(image_construction_data& construction_data, image_data& out) {
	std::vector<std::uint8_t> decompressed;
	bitwise_readable_stream stream{ { zlib_stream->data(), zlib_stream->data() + zlib_stream->size() } };
	decompress(decompressed, stream);
	scanline_data scanlines{ construction_data, { decompressed.data(), decompressed.data() + decompressed.size() } };
	scanlines.reconstruct_data();
	scanlines.write_to(out);
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

struct chunk_registration_struct {
	chunk_registration_struct() {
		register_chunk_types();
	}
} chunk_registration_struct_instance;