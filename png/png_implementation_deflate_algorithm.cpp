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