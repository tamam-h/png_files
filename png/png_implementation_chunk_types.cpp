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
	dimension_struct first_dimension{ (in.width - 1 >> 3) * dimensions[reduced_image_number][7][7].width, (in.height - 1 >> 3) * dimensions[reduced_image_number][7][7].height };
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

std::uint_fast64_t get_pixel(const std::vector<std::vector<std::uint8_t>>& reduced_image, std::uint_fast32_t i, std::uint_fast32_t j, std::uint_fast8_t bytes_per_pixel) {
	assert(bytes_per_pixel > 0 && "bytes_per_pixel should not be equal to zero");
	assert(i < reduced_image.size() && "invalid index i");
	const std::vector<std::uint8_t>& scanline{ reduced_image[i] };
	std::uint_fast64_t byte_index{ static_cast<std::uint_fast64_t>(bytes_per_pixel) * j }, bit_index{ byte_index & 0b111 };
	byte_index >>= 3;
	++byte_index;
	assert(byte_index < scanline.size() && "out of range");
	const std::uint8_t* position{ scanline.data() + byte_index };
	if (bytes_per_pixel < 0b1000) { return (*position & ((1 << bytes_per_pixel) - 1 << 8 - bit_index - bytes_per_pixel)) >> 8 - bit_index - bytes_per_pixel; }
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

void scanline_data::write_to(image_data& out) {

}

void scanline_data::reconstruct_data() {
	if (construction_data.uses_interlacing) {
		bool is_set{ 0 };
		std::uint_fast8_t filter_method;
		for (std::vector<std::vector<std::uint8_t>>& i : scanlines) {
			for (std::vector<std::uint8_t>& j : i) {
				for (std::uint8_t k : j) {
					assert(j.size() > 1 && "scanlines should have a size of two or more: filter type and pixel data");
					if (!is_set) {
						filter_method = k;
						is_set = 1;
					} else if (k != filter_method) {
						// https://www.w3.org/TR/2003/REC-PNG-20031110/ section 9.1
						throw std::runtime_error{ "all filter types should be the same if the image uses interlacing" };
					}
				}
			}
		}
	}
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
			}
		}
	}
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