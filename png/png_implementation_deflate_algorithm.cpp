#include "pch.h"
#include "png_implementation_deflate_algorithm.hpp"

huffman_code_t bit_reverse(huffman_code_t value, code_length_t length) {
	assert(length <= 15);
	assert(value < static_cast<huffman_code_t>(1 << length));
	static const std::vector<std::vector<huffman_code_t>> lookup_table{
		[]() -> std::vector<std::vector<huffman_code_t>> {
			std::vector<std::vector<huffman_code_t>> table(16);
			for (code_length_t i{ 0 }; i < 16; ++i) {
				table[i].resize(static_cast<std::size_t>(1) << i);
				for (huffman_code_t j{ 0 }; j < static_cast<huffman_code_t>(1 << i); ++j) {
					huffman_code_t acc{ 0 }, to_be_reversed{ j }, size{ i };
					while (size--) {
						acc <<= 1;
						acc |= to_be_reversed & 1;
						to_be_reversed >>= 1;
					}
					table[i][j] = acc;
				}
			}
			return table;
		} ()
	};
	return lookup_table[length][value];
}

huffman_code_table::huffman_code_table() : max_code_length{ 0 } {}

huffman_code_table::huffman_code_table(std::span<const code_length_t> code_lengths) {
	assert(code_lengths.size());
	max_code_length = *std::max_element(code_lengths.begin(), code_lengths.end());
	assert(max_code_length <= 15);
	// https://www.rfc-editor.org/rfc/rfc1951.pdf section 3.2.2
	std::vector<std::uint_fast16_t> code_length_counts;
	code_length_counts.resize(max_code_length + 1);
	for (code_length_t i : code_lengths) {
		if (i) { ++code_length_counts[i]; }
	}
	std::vector<huffman_code_t> next_code;
	huffman_code_t code{ 0 };
	next_code.resize(max_code_length + 1);
	for (code_length_t code_length{ 0 }; code_length + 1 <= max_code_length; ++code_length) {
		code += code_length_counts[code_length];
		next_code[code_length + 1] = code <<= 1;
	}
	std::vector<huffman_code_t> codes(code_lengths.size());
	for (huffman_code_t symbol{ 0 }; symbol < codes.size(); ++symbol) {
		code_length_t temp{ code_lengths[symbol] };
		if (temp) { codes[symbol] = next_code[temp]++; }
	}
	// making of the lookup table
	table.resize(static_cast<std::size_t>(1) << max_code_length);
	for (huffman_code_t symbol{ 0 }; symbol < codes.size(); ++symbol) {
		code_length_t current_code_length{ code_lengths[symbol] };
		if (current_code_length) {
			huffman_code_t reversed_code{ bit_reverse(codes[symbol], current_code_length) };
			for (huffman_code_t i{ 0 }; i < static_cast<huffman_code_t>(1 << (max_code_length - current_code_length)); ++i) {
				table[i << current_code_length | reversed_code] = symbol;
			}
		}
	}
}

huffman_code_t huffman_code_table::peek(const bitwise_readable_stream& current_bitwise_readable_stream) const noexcept {
	assert(max_code_length);
	return table[current_bitwise_readable_stream.zero_extended_peek(max_code_length)];
}

std::uint_fast16_t get_length(bitwise_readable_stream& compressed, huffman_code_t code) {
	assert(code >= 257 && code <= 287);
	if (code < 257 || code > 285) { throw std::runtime_error{ "" }; }
	static const std::uint_fast8_t extra_bit_counts[]{ 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5, 0 };
	static const std::uint_fast16_t lengths[]{ 3, 4, 5, 6, 7, 8, 9, 10, 11, 13, 15, 17, 19, 23, 27, 31, 35, 43, 51, 59, 67, 83, 99, 115, 131, 163, 195, 227, 258 };
	std::uint_fast16_t acc{ lengths[code - 257] };
	std::uint_fast8_t extra_bit_count{ extra_bit_counts[code - 257] };
	if (extra_bit_count) {
		if (!compressed.can_advance(extra_bit_count)) { throw std::out_of_range{ "" }; }
		acc += compressed.peek(extra_bit_count);
		compressed.advance(extra_bit_count);
	}
	return acc;
}

std::uint_fast16_t get_distance(bitwise_readable_stream& compressed, huffman_code_t code) {
	assert(code >= 0 && code <= 31);
	if (code > 29) { throw std::runtime_error{ "" }; }
	static const std::uint_fast8_t extra_bit_counts[]{ 0, 0, 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10, 10, 11, 11, 12, 12, 13, 13 };
	static const std::uint_fast16_t distances[]{ 1, 2, 3, 4, 5, 7, 9, 13, 17, 25, 33, 49, 65, 97, 129, 193, 257, 385, 513, 769, 1025, 1537, 2049, 3073, 4097, 6145, 8193, 12289, 16385, 24577 };
	std::uint_fast16_t acc{ distances[code] };
	std::uint_fast8_t extra_bit_count{ extra_bit_counts[code] };
	if (extra_bit_count) {
		if (!compressed.can_advance(extra_bit_count)) { throw std::out_of_range{ "" }; }
		acc += compressed.peek(extra_bit_count);
		compressed.advance(extra_bit_count);
	}
	return acc;
}

void handle_uncompressed_block(std::vector<std::uint8_t>& out, bitwise_readable_stream& compressed) {
	compressed.advance_to_byte();
	if (!compressed.can_advance(32)) { throw std::out_of_range{ "" }; }
	const std::uint8_t* position{ compressed.get_position() };
	std::uint_fast16_t length{ static_cast<std::uint_fast16_t>(position[0] | position[1] << 8) }, complement_of_length{ static_cast<std::uint_fast16_t>(position[2] | position[3] << 8) };
	if ((~length & 0xFFFF) != complement_of_length) { throw std::runtime_error{ "" }; }
	compressed.advance(32);
	if (!compressed.can_advance(length * 8)) { throw std::out_of_range{ "" }; }
	out.resize(out.size() + length);
	std::memcpy(out.data() + out.size() - length, compressed.get_position(), length);
	compressed.advance(length * 8);
}

void handle_code_length_code(std::vector<code_length_t>& out, huffman_code_t symbol, bitwise_readable_stream& compressed) {
	assert(symbol < 19);
	if (symbol <= 15) {
		out.push_back(static_cast<code_length_t>(symbol));
		return;
	}
	std::uint_fast8_t bit_counts[]{ 2, 3, 7 }, bit_count{ bit_counts[symbol - 16] };
	if (!compressed.can_advance(bit_count)) { throw std::out_of_range{ "" }; }
	std::uint_fast8_t length{ static_cast<std::uint_fast8_t>(compressed.peek(bit_count)) };
	compressed.advance(bit_count);
	switch (symbol) {
	case 16:
		length += 3;
		out.reserve(out.size() + length);
		if (out.empty()) { throw std::runtime_error{ "" }; }
		while (length--) { out.push_back(out.back()); }
		return;
	case 17:
		length += 3;
		out.reserve(out.size() + length);
		while (length--) { out.push_back(0); }
		return;
	case 18:
		length += 11;
		out.reserve(out.size() + length);
		while (length--) { out.push_back(0); }
		return;
	}
}

void huffman_code_reader::set_table() {
	table = std::span<const std::uint8_t>{ code_lengths.data(), code_lengths.data() + code_lengths.size() };
}

huffman_code_t huffman_code_reader::read(bitwise_readable_stream& compressed) const {
	huffman_code_t code{ table.peek(compressed) };
	code_length_t code_length{ code_lengths[code] };
	if (!compressed.can_advance(code_length)) { throw std::out_of_range{ "" }; }
	compressed.advance(code_length);
	return code;
}

void decompress(std::vector<std::uint8_t>& out, bitwise_readable_stream& compressed) {
	static const huffman_code_reader fixed_huffman_codes_reader{
		[]() -> huffman_code_reader {
			huffman_code_reader temp; temp.code_lengths.resize(288);
			std::memset(temp.code_lengths.data(), 8, 144);
			std::memset(temp.code_lengths.data() + 144, 9, 112);
			std::memset(temp.code_lengths.data() + 256, 7, 24);
			std::memset(temp.code_lengths.data() + 280, 8, 8);
			temp.set_table();
			return temp;
		} ()
	};
	static const huffman_code_reader fixed_distance_codes_reader{
		[]() -> huffman_code_reader {
			huffman_code_reader temp; temp.code_lengths = std::vector<code_length_t>(32, 5);
			temp.set_table();
			return temp;
		} ()
	};
	huffman_code_reader dynamic_huffman_codes_reader, dynamic_distance_codes_reader;
	std::uint_fast16_t is_last_block{ 0 }, block_type{ 0 };
	// https://www.rfc-editor.org/rfc/rfc1951.pdf section 3.2.3
	while (!is_last_block) {
		if (!compressed.can_advance(3)) { throw std::out_of_range{ "" }; }
		is_last_block = compressed.peek(3);
		compressed.advance(3);
		block_type = is_last_block & 0b110;
		is_last_block &= 0b1;
		if (block_type == 0b110) { throw std::runtime_error{ "" }; }
		if (block_type == 0b000) {
			handle_uncompressed_block(out, compressed);
			continue;
		}
		const huffman_code_reader* huffman_codes_reader{ &fixed_huffman_codes_reader }, * distance_codes_reader{ &fixed_distance_codes_reader };
		if (block_type == 0b100) {
			if (!compressed.can_advance(14)) { throw std::out_of_range{ "" }; }
			std::uint_fast16_t literal_length_code_count{ compressed.peek(5) };
			literal_length_code_count += 257;
			compressed.advance(5);
			std::uint_fast16_t distance_code_count{ compressed.peek(5) };
			++distance_code_count;
			compressed.advance(5);
			std::uint_fast16_t code_length_code_count{ compressed.peek(4) };
			code_length_code_count += 4;
			compressed.advance(4);
			huffman_code_reader code_length_code_reader; code_length_code_reader.code_lengths.resize(19);
			static const std::uint_fast8_t symbol_index[]{ 16, 17, 18, 0, 8, 7, 9, 6, 10, 5, 11, 4, 12, 3, 13, 2, 14, 1, 15 };
			for (std::uint_fast16_t code_length_code_index{ 0 }; code_length_code_index < code_length_code_count; ++code_length_code_index) {
				if (!compressed.can_advance(3)) { throw std::out_of_range{ "" }; }
				std::uint_fast16_t code_length{ compressed.peek(3) };
				compressed.advance(3);
				code_length_code_reader.code_lengths[symbol_index[code_length_code_index]] = code_length;
			}
			code_length_code_reader.set_table();
			const std::uint_fast16_t count{ literal_length_code_count + distance_code_count };
			std::vector<code_length_t> decompressed_code_lengths; decompressed_code_lengths.reserve(count);
			while (1) {
				handle_code_length_code(decompressed_code_lengths, code_length_code_reader.read(compressed), compressed);
				if (decompressed_code_lengths.size() > count) { throw std::runtime_error{ "" }; }
				if (decompressed_code_lengths.size() == count) { break; }
			}
			dynamic_huffman_codes_reader.code_lengths.clear();
			dynamic_huffman_codes_reader.code_lengths.resize(288, 0);
			std::memcpy(dynamic_huffman_codes_reader.code_lengths.data(), decompressed_code_lengths.data(), literal_length_code_count);
			dynamic_huffman_codes_reader.set_table();
			huffman_codes_reader = &dynamic_huffman_codes_reader;
			dynamic_distance_codes_reader.code_lengths.clear();
			dynamic_distance_codes_reader.code_lengths.resize(32, 0);
			std::memcpy(dynamic_distance_codes_reader.code_lengths.data(), decompressed_code_lengths.data() + literal_length_code_count, distance_code_count);
			dynamic_distance_codes_reader.set_table();
			distance_codes_reader = &dynamic_distance_codes_reader;
		}
		while (1) {
			huffman_code_t literal_length_code{ huffman_codes_reader->read(compressed) };
			if (literal_length_code < 256) {
				out.push_back(literal_length_code);
			} else if (literal_length_code == 256) {
				break;
			} else {
				std::uint_fast16_t length{ get_length(compressed, literal_length_code) };
				std::uint_fast16_t distance{ get_distance(compressed, distance_codes_reader->read(compressed)) };
				if (out.empty()) { throw std::runtime_error{ "" }; }
				--distance;
				while (length--) { out.push_back(out.rbegin()[distance]); }
			}
		}
	}
}

void compress(std::vector<std::uint8_t>& out, std::span<const std::uint8_t> uncompressed) {
	std::size_t size{ uncompressed.size() };
	const std::uint8_t* source{ uncompressed.data() };
	while (size) {
		if (size <= UINT16_MAX) {
			out.reserve(out.size() + size + 5);
			std::uint8_t* destination{ out.data() + out.size() };
			out.resize(out.size() + size + 5);
			destination[0] = 1;
			++destination;
			destination[0] = size & 0xFF;
			destination[1] = (size & 0xFF00) >> 8;
			destination[2] = ~destination[0];
			destination[3] = ~destination[1];
			destination += 4;
			std::memcpy(destination, source, size);
			size = 0;
		} else {
			out.reserve(out.size() + UINT16_MAX + 5);
			std::uint8_t* destination{ out.data() + out.size() };
			out.resize(out.size() + UINT16_MAX + 5);
			destination[0] = 0;
			++destination;
			destination[0] = 0xFF;
			destination[1] = 0xFF;
			destination[2] = 0x00;
			destination[3] = 0x00;
			destination += 4;
			std::memcpy(destination, source, UINT16_MAX);
			size -= UINT16_MAX;
			source += UINT16_MAX;
		}
	}
}