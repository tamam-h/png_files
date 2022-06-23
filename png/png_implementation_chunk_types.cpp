#include "pch.h"
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
	static const std::set<std::pair<std::uint_fast8_t, std::uint_fast8_t>> allowed_colour_type_bit_depth_pairs{
		{ 0, 1 }, { 0, 2 }, { 0, 4 }, { 0, 8 }, { 0, 16 }, { 2, 8 }, { 2, 16 }, { 3, 1 }, { 3, 2 }, { 3, 4 }, { 3, 8 }, { 4, 8 }, { 4, 16 }, { 6, 8 }, { 6, 16 } };
	if (!allowed_colour_type_bit_depth_pairs.contains({ colour_type, bit_depth })) { throw std::runtime_error{ "invalid pair of colour type and bit depth" }; }
	return std::unique_ptr<chunk_base>{ new IHDR_chunk{ width, height, bit_depth, colour_type, compression_method, filter_method, interlace_method } };
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
	std::size_t chunk_size{ in.size() };
	const std::uint8_t* position{ in.data() };
	std::vector<truecolour_8> palette; palette.reserve(in.size() / 3);
	while (chunk_size -= 3) {
		palette.emplace_back();
		palette.back().red = *position++;
		palette.back().green = *position++;
		palette.back().blue = *position++;
	}
	return std::unique_ptr<chunk_base>{ new PLTE_chunk{ std::move(palette) } };
}

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