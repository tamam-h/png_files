#include "pch.h"
#include "png.hpp"
#include "png_implementation_ancillary_chunk_types.hpp"
#include "png_implementation_chunk_factory.hpp"
#include "png_implementation_chunk_types.hpp"
#include "png_implementation_image_data.hpp"
#include "png_implementation_macros.hpp"
#include "png_implementation_pixel_cast.hpp"
#include "png_implementation_pixel_types.hpp"
#include "png_implementation_zlib_stream.hpp"

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

png::png() : pointer_to_implementation{ new implementation } {}

png::png(std::span<const std::uint8_t> in) : pointer_to_implementation{ new implementation } {
	static const int chunk_registration{
		[]() -> int {
			register_chunk_types();
			register_ancillary_chunk_types();
			return 0;
		} ()
	};
	std::vector<std::unique_ptr<chunk_base>> chunks;
	create_chunks(chunks, in);
	set_image_data(*pointer_to_implementation, chunks);
}

png::~png() {}

std::uint_fast32_t& png::get_gamma() noexcept {
	return pointer_to_implementation->colour_info.gamma;
}

const std::uint_fast32_t& png::get_gamma() const noexcept {
	return pointer_to_implementation->colour_info.gamma;
}

#define GET_WIDTH_AND_HEIGHT_CASE(type)\
case pixel_type_hash::type:\
	{\
		const std::vector<std::vector<type>>& arr{ pointer_to_implementation->get_array<type>() };\
		height = static_cast<std::uint_fast32_t>(arr.size());\
		assert(height && "height should not be zero");\
		assert(height <= INT32_MAX && "height is too big to write");\
		width = static_cast<std::uint_fast32_t>(arr[0].size());\
		assert(width && "width should not be zero");\
		assert(width <= INT32_MAX && "width is too big to write");\
	}\
	break;

void png::write_to(std::vector<std::uint8_t>& out) const {
	const pixel_type_hash image_info{ to_pixel_type_hash(get_pixel_type()) };
	std::uint_fast32_t width{}, height{}, gamma{ get_gamma() };
	switch (image_info) {
	APPLY_TO_WRITABLE_PIXEL_TYPES(GET_WIDTH_AND_HEIGHT_CASE)
	default:
		assert(0 && "unknown pixel type");
	}
	zlib_stream_handler zlib_stream;
	const std::uint_fast8_t interlace_method{
		[this, image_info, width, height, &zlib_stream]() -> std::uint_fast8_t {
			scanline_data scanlines{ *pointer_to_implementation, image_info, width, height };
			std::uint_fast8_t interlace_method{ scanlines.filter_data() };
			scanlines.write_to(zlib_stream.uncompressed_data);
			return interlace_method;
		} ()
	};
	zlib_stream.compress();
	zlib_stream.uncompressed_data.clear();
	zlib_stream.uncompressed_data.shrink_to_fit();
	const std::uint_fast64_t file_size{
		[&zlib_stream, gamma]() -> std::uint_fast64_t {
			std::uint_fast64_t div{ zlib_stream.compressed_data.size() / INT32_MAX }, rem{ zlib_stream.compressed_data.size() % INT32_MAX };
			std::uint_fast64_t acc{ 45ull + div * (INT32_MAX + 12ull) };
			if (rem) { acc += rem + 12ull; }
			if (gamma) { acc += 16; }
			return acc;
		} ()
	};
	out.reserve(out.size() + file_size);
	std::uint8_t* position{ out.data() + out.size() };
	out.resize(out.size() + file_size);
	write(position, { 64, file_header });
	write(position, { 32, 13 });
	write(position, { 32, IHDR_chunk::type });
	write(position, { 32, width });
	write(position, { 32, height });
	write(position, { 8, static_cast<std::uint_fast64_t>(image_info) & 0b1'1111 });
	write(position, { 8, (static_cast<std::uint_fast64_t>(image_info) & 0b1110'0000) >> 5 });
	write(position, { 8, 0 });
	write(position, { 8, 0 });
	write(position, { 8, interlace_method });
	write(position, { 32, crc32({ position - 17, position }) });
	if (gamma) {
		write(position, { 32, 4 });
		write(position, { 32, gAMA_chunk::type });
		write(position, { 32, gamma });
		write(position, { 32, crc32({ position - 8, position }) });
	}
	const std::uint8_t* const end_of_zlib_stream{ zlib_stream.compressed_data.data() + zlib_stream.compressed_data.size() };
	std::uint_fast64_t zlib_stream_size{ zlib_stream.compressed_data.size() };
	while (zlib_stream_size) {
		std::uint_fast64_t writing_size{ std::min(zlib_stream_size, static_cast<std::uint_fast64_t>(INT32_MAX)) };
		write(position, { 32, writing_size });
		write(position, { 32, IDAT_chunk::type });
		std::memcpy(position, end_of_zlib_stream - zlib_stream_size, writing_size);
		position += writing_size;
		write(position, { 32, crc32({ position - writing_size - 4, position }) });
		zlib_stream_size -= writing_size;
	}
	write(position, { 32, 0 });
	write(position, { 32, IEND_chunk::type });
	write(position, { 32, crc32({ position - 4, position }) });
}