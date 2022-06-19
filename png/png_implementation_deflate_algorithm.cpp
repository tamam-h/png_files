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

huffman_code_reader::huffman_code_reader() : max_code_length{ 0 } {}

huffman_code_reader::huffman_code_reader(std::span<const code_length_t> code_lengths) {
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

huffman_code_t huffman_code_reader::peek(const bitwise_readable_stream& current_bitwise_readable_stream) const noexcept {
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