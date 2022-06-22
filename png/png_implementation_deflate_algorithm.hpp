#pragma once
#include <cstdint>
#include "png_implementation_bitwise_readable_stream.hpp"
#include <span>
#include <vector>

// function that compresses std::span<const char> and appends to std::vector<std::uint8_t>& out
// output does not necessarily have less size than uncompressed data
void compress(std::vector<std::uint8_t>& out, std::span<const std::uint8_t> uncompressed);

// function that decompresses bits read from a bitwise_readable_stream called compressed and appends to std::vector<std::uint8_t>& out
// will throw std::exception if there is a problem
void decompress(std::vector<std::uint8_t>& out, bitwise_readable_stream& compressed);

using code_length_t = std::uint_fast8_t;
using huffman_code_t = std::uint_fast16_t;

// reverses order of std::uint_fast8_t length least significant bits in std::uint_fast16_t value
// throws if other bits are not 0
// assumes length is less than or equal to 15
huffman_code_t bit_reverse(huffman_code_t value, code_length_t length);

// https://www.rfc-editor.org/rfc/rfc1951.pdf section 3.2
// instead of a tree, the struct uses a lookup table
struct huffman_code_table {
	// sets max_code_length to zero
	huffman_code_table();
	// code_lengths.size() gives number of symbols in alphabet
	// first symbol is 0, last symbol is code_lengths.size()
	// max element in code_lengths should be less than or equal to 15
	// code_lengths does not have to exist for the lifetime of a huffman_code_table
	// code_lengths cannot be empty
	huffman_code_table(std::span<const code_length_t> code_lengths);
	// reads a symbol from png::bitwise_readable_stream& current_bitwise_readable_stream
	// max_code_length is assumed to be not zero
	huffman_code_t peek(const bitwise_readable_stream& current_bitwise_readable_stream) const noexcept;
private:
	std::vector<huffman_code_t> table;
	code_length_t max_code_length;
};

// https://www.rfc-editor.org/rfc/rfc1951.pdf section 3.2.5
// assumes code is between 257 and 285 inclusive
// throws if code is 286 or 287
// throws if can't read anymore from compressed
// advances position in stream after reading
std::uint_fast16_t get_length(bitwise_readable_stream& compressed, huffman_code_t code);

// https://www.rfc-editor.org/rfc/rfc1951.pdf section 3.2.5
// assumes code is between 0 and 29 inclusive
// throws if code is 30 or 31
// throws if can't read anymore from compressed
// advances position in stream after reading
std::uint_fast16_t get_distance(bitwise_readable_stream& compressed, huffman_code_t code);

// https://www.rfc-editor.org/rfc/rfc1951.pdf section 3.2.3 and 3.2.4
// throws if can't read anymore from compressed
// throws if (~LEN & 0xFFFF) != NLEN
// advances to byte before reading LEN
void handle_uncompressed_block(std::vector<std::uint8_t>& out, bitwise_readable_stream& compressed);

// https://www.rfc-editor.org/rfc/rfc1951.pdf section 3.2.7
// throws if can't read anymore from compressed
// assumes symbol is less than 19
// throws if out is empty and symbol is 16
void handle_code_length_code(std::vector<code_length_t>& out, huffman_code_t symbol, bitwise_readable_stream& compressed);

// struct that facilitates reading a huffman_code_t from a bitwise_readable_stream&
struct huffman_code_reader {
	// used to specify code lengths of alphabet
	// size of vector is size of alphabet
	std::vector<code_length_t> code_lengths;
	// function to construct table after code_lengths is set to a value
	void set_table();
	// reads a huffman_code_t from a bitwise_readable_stream&
	// throws if there is a problem with reading
	huffman_code_t read(bitwise_readable_stream& compressed) const;
private:
	huffman_code_table table;
};