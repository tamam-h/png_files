#include "pch.h"
#include "png_implementation_chunk_factory.hpp"
#include "png_implementation_image_data.hpp"

std::uint_fast32_t crc32(std::span<const std::uint8_t> in) {
	// https://www.w3.org/TR/2003/REC-PNG-20031110/ annex d
	static const std::vector<std::uint_fast32_t> crc_table{
		[]() -> std::vector<std::uint_fast32_t> {
			std::vector<std::uint_fast32_t> table(256);
			for (std::uint_fast16_t n{ 0 }; n < 256; ++n) {
				std::uint_fast32_t c{ n };
				for (std::uint_fast8_t k{ 0 }; k < 8; ++k) {
					c = (c & 1) ? (0xedb8'8320 ^ (c >> 1)) : (c >> 1);
				}
				table[n] = c;
			}
			return table;
		} ()
	};
	std::uint_fast32_t c{ 0xffff'ffff };
	for (std::uint8_t i : in) { c = crc_table[(c ^ i) & 0xff] ^ (c >> 8); }
	return c ^ 0xffff'ffff;
}

bool is_valid_chunk_type(chunk_type_t in) {
	if ((in & 0xffff'ffff) != in) { return 0; }
	int count{ 4 };
	while (count--) {
		chunk_type_t temp{ in & 0xff };
		if (temp < 65 || (temp > 90 && temp < 97) || temp > 122) { return 0; }
		in >>= 8;
	}
	return 1;
}

bool is_critical_chunk_type(chunk_type_t in) {
	assert(is_valid_chunk_type(in));
	return !(0b0010'0000'0000'0000'0000'0000'0000'0000 & in);
}

bool reserved_bit_set(chunk_type_t in) {
	assert(is_valid_chunk_type(in));
	return 0b0000'0000'0000'0000'0010'0000'0000'0000 & in;
}

std::map<chunk_type_t, std::unique_ptr<chunk_base> (*)(std::span<const std::uint8_t> in, const std::vector<std::unique_ptr<chunk_base>>& chunks)> chunk_type_registry;

void register_chunk_type(chunk_type_t type, std::unique_ptr<chunk_base>(*create_chunk)(std::span<const std::uint8_t> in, const std::vector<std::unique_ptr<chunk_base>>& chunks)) {
	assert(is_valid_chunk_type(type));
	assert(!reserved_bit_set(type));
	chunk_type_registry[type] = create_chunk;
}

void clear_chunk_type_registry() {
	chunk_type_registry.clear();
}

chunk_base::~chunk_base() {}

unknown_chunk::unknown_chunk(std::span<const std::uint8_t> content, chunk_type_t chunk_type) : chunk_type{ chunk_type } {}

chunk_type_t unknown_chunk::get_type() const {
	return chunk_type;
}

void unknown_chunk::set_image_data(image_construction_data& construction_data, image_data& out) {}

void assert_can_read(const std::uint8_t* position, const std::span<const std::uint8_t>& in) {
	assert(position >= in.data());
	if (position >= in.data() + in.size()) {
		throw std::out_of_range{ "can\'t read from that position" };
	}
}

std::uint_fast32_t read_4(const std::uint8_t*& position, const std::span<const std::uint8_t>& in) {
	assert(position >= in.data());
	assert_can_read(position + 3, in);
	std::uint_fast32_t acc{ static_cast<std::uint_fast32_t>(position[0]) << 24
		| static_cast<std::uint_fast32_t>(position[1]) << 16
		| static_cast<std::uint_fast32_t>(position[2]) << 8
		| static_cast<std::uint_fast32_t>(position[3]) };
	position += 4;
	return acc;
}

std::uint_fast8_t read_1(const std::uint8_t*& position, const std::span<const std::uint8_t>& in) {
	assert(position >= in.data());
	assert_can_read(position, in);
	return *position++;
}

void write_4(std::uint_fast32_t value, std::uint8_t*& position, const std::span<std::uint8_t>& out) {
	assert(position >= out.data());
	assert_can_read(position + 3, out);
	position[0] = (value & 0xff00'0000) >> 24;
	position[1] = (value & 0x00ff'0000) >> 16;
	position[2] = (value & 0x0000'ff00) >> 8;
	position[3] = (value & 0x0000'00ff);
	position += 4;
}

std::uint_fast64_t read_8(const std::uint8_t*& position, const std::span<const std::uint8_t>& in) {
	assert(position >= in.data());
	assert_can_read(position + 7, in);
	std::uint_fast64_t acc{ static_cast<std::uint_fast64_t>(position[0]) << 56
		| static_cast<std::uint_fast64_t>(position[1]) << 48
		| static_cast<std::uint_fast64_t>(position[2]) << 40
		| static_cast<std::uint_fast64_t>(position[3]) << 32
		| static_cast<std::uint_fast64_t>(position[4]) << 24
		| static_cast<std::uint_fast64_t>(position[5]) << 16
		| static_cast<std::uint_fast64_t>(position[6]) << 8
		| static_cast<std::uint_fast64_t>(position[7]) };
	position += 8;
	return acc;
}

void write_8(std::uint_fast64_t value, std::uint8_t*& position, const std::span<std::uint8_t>& out) {
	assert(position >= out.data());
	assert_can_read(position + 7, out);
	position[0] = static_cast<std::uint8_t>((value & 0xff00'0000'0000'0000) >> 56);
	position[1] = static_cast<std::uint8_t>((value & 0x00ff'0000'0000'0000) >> 48);
	position[2] = static_cast<std::uint8_t>((value & 0x0000'ff00'0000'0000) >> 40);
	position[3] = static_cast<std::uint8_t>((value & 0x0000'00ff'0000'0000) >> 32);
	position[4] = static_cast<std::uint8_t>((value & 0x0000'0000'ff00'0000) >> 24);
	position[5] = static_cast<std::uint8_t>((value & 0x0000'0000'00ff'0000) >> 16);
	position[6] = static_cast<std::uint8_t>((value & 0x0000'0000'0000'ff00) >> 8);
	position[7] = static_cast<std::uint8_t>((value & 0x0000'0000'0000'00ff));
	position += 8;
}

void write_1(std::uint_fast8_t value, std::uint8_t*& position, const std::span<std::uint8_t>& out) {
	assert(position >= out.data());
	assert_can_read(position, out);
	*position++ = value;
}

void create_chunks(std::vector<std::unique_ptr<chunk_base>>& out, std::span<const std::uint8_t> in) {
	const std::uint8_t* position{ in.data() };
	// https://www.w3.org/TR/2003/REC-PNG-20031110/ section 5.2
	if (read_8(position, in) != (137ull << 56 | 80ull << 48 | 78ull << 40 | 71ull << 32 | 13ull << 24 | 10ull << 16 | 26ull << 8 | 10ull)) { throw std::runtime_error{ "file header is different than expected" }; }
	while (1) {
		std::uint32_t chunk_size{ read_4(position, in) };
		// https://www.w3.org/TR/2003/REC-PNG-20031110/ section 5.3
		if (chunk_size > INT32_MAX) { throw std::runtime_error{ "chunk size is larger than allowed" }; }
		chunk_type_t chunk_type{ static_cast<chunk_type_t>(read_4(position, in)) };
		if (!is_valid_chunk_type(chunk_type)) { throw std::runtime_error{ "chunk type is not valid" }; }
		assert_can_read(position + chunk_size - 1, in);
		std::uint_fast32_t crc{ crc32({ position - 4, position + chunk_size - 4 }) };
		position += chunk_size - 4;
		if (crc != read_4(position, in)) { throw std::runtime_error{ "stated CRC-32 does not match calculated CRC-32" }; }
		decltype(chunk_type_registry)::iterator it{ chunk_type_registry.find(chunk_type) };
		if (it == chunk_type_registry.end()) {
			if (is_critical_chunk_type(chunk_type)) { throw std::runtime_error{ "found critical chunk but chunk type is unknown" }; }
			out.emplace_back(new unknown_chunk{ { position - chunk_size - 4, position - 4 }, chunk_type });
		} else {
			out.emplace_back(it->second({ position - chunk_size - 4, position - 4 }, out));
		}
		if (chunk_type == end_chunk_type) { break; }
	}
}