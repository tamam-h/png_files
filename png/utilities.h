#pragma once
#include <span>
#include <vector>
#include <cstdint>
#include <memory>

namespace png {
	// struct that facilitates reading a span of std::uint8_t bit by bit from least significant bit of start byte to most significant bit of end byte
	// the range specified should exist for the lifetime of a bitwise_readable_stream
	struct bitwise_readable_stream {
		// begin points to first std::uint8_t in range, end points to std::uint8_t after last element in range
		// first bit to be read is least significant bit of std::uint8_t pointed to by begin
		bitwise_readable_stream(const std::uint8_t* begin, const std::uint8_t* end);
		// loses information about position in stream
		explicit operator std::span<const std::uint8_t>() const noexcept;
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
		~bitwise_readable_stream();
	private:
		struct data;
		std::unique_ptr<data> pointer_to_data;
	};
}