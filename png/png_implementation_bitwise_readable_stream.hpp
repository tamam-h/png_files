#pragma once
#include <cstdint>
#include <span>

// struct that facilitates reading a span of std::uint8_t bit by bit from least significant bit of start byte to most significant bit of end byte
struct bitwise_readable_stream {
	// first bit to be read is least significant bit of std::uint8_t pointed to by range.begin()
	// the range specified should exist for the lifetime of a bitwise_readable_stream
	bitwise_readable_stream(std::span<const std::uint8_t> range);
	// in range position in stream is between least significant bit of begin byte and most significant bit of end byte inclusive
	// assumes all bits to be read are in range
	// does not advance position
	// assumes bits is less than or equal to 16
	std::uint_fast16_t peek(unsigned int bits) const noexcept;
	// in range position in stream is between least significant bit of begin byte and bit after most significant bit of end byte inclusive
	// advances position in stream by unsigned int bits
	// new position can be anywhere in range
	void advance(unsigned int bits) noexcept;
	// in range position in stream is between least significant bit of begin byte and bit after most significant bit of end byte inclusive
	// returns if position will be in range if advanced by unsigned int bits
	bool can_advance(unsigned int bits) const noexcept;
	// in range position in stream is between least significant bit of begin byte and most significant bit of end byte inclusive
	// reads unsigned int bits from position in stream if bits are in range
	// fills the remaining bits with zero
	// assumes position is either least significant bit of begin or beyond
	// assumes bits is less than or equal to 16
	std::uint_fast16_t zero_extended_peek(unsigned int bits) const noexcept;
	// in range position in stream is between least significant bit of begin byte and bit after most significant bit of end byte inclusive
	// advances position in stream bit by bit until at a byte boundary
	// distance advanced might be zero bits
	// new position can be anywhere in range
	void advance_to_byte() noexcept;
	// assumes position in stream is at a byte boundary
	const std::uint8_t* get_position() const noexcept;
private:
	const std::uint8_t* const begin, * const end;
	unsigned int byte_index, bit_index;
	struct functions;
};