#include "pch.h"
#include "png_implementation_bitwise_readable_stream.hpp"

bitwise_readable_stream::bitwise_readable_stream(std::span<const std::uint8_t> range) : begin{ range.data() }, end{ range.data() + range.size() }, byte_index{ 0 }, bit_index{ 0 } {}

bool bitwise_readable_stream::can_advance(unsigned int bits) const noexcept {
	unsigned int bit_index{ this->bit_index + bits };
	unsigned int byte_index{ this->byte_index + bit_index / 8 };
	bit_index %= 8;
	if (byte_index < end - begin) { return 1; }
	if (byte_index == end - begin) { return bit_index == 0; }
	return 0;
}

struct bitwise_readable_stream::functions {
	// access_function is a functor that returns the value of in.begin[ui] or some other value
	template <class functor_type> static std::uint_fast16_t peek(const bitwise_readable_stream& in, unsigned int bits, const functor_type& access_function) noexcept
		requires requires(unsigned int ui) { { access_function(ui) } -> std::same_as<std::uint8_t>; } {
		std::uint_fast16_t acc{ 0 };
		unsigned int byte_index{ in.byte_index }, bit_index{ in.bit_index };
		int put_index{ 0 };
		while (bits) {
			if (bits >= 8 - bit_index) {
				acc |= (access_function(byte_index) & (0b1111'1111 << bit_index)) >> bit_index << put_index;
				bits -= 8 - bit_index;
				put_index += 8 - bit_index;
				bit_index = 0;
				++byte_index;
			} else {
				unsigned int temp{ access_function(byte_index) };
				temp >>= bit_index;
				acc |= (temp & (0b1111'1111 >> (8 - static_cast<int>(bits)))) << put_index;
				bits = 0;
			}
		}
		return acc;
	}
};

std::uint_fast16_t bitwise_readable_stream::peek(unsigned int bits) const noexcept {
	assert(can_advance(bits) && "can\'t peek bits if can\'t advance that number of bits");
	assert(bits <= 16 && "not implemented for when trying to peek more than 16 bits");
	const auto get_index{
		[this](unsigned int index) -> std::uint8_t {
			return begin[index];
		}
	};
	return functions::peek(*this, bits, get_index);
}

void bitwise_readable_stream::advance(unsigned int bits) noexcept {
	assert(can_advance(bits) && "can\'t advance that number of bits");
	bit_index += bits;
	byte_index += bit_index >> 3;
	bit_index &= 0b111;
}

std::uint_fast16_t bitwise_readable_stream::zero_extended_peek(unsigned int bits) const noexcept {
	assert(bits <= 16 && "not implemented for trying to zero_extended_peek more than 16 bits");
	if (can_advance(bits)) { return peek(bits); }
	const auto get_index{
		[this](unsigned int index) -> std::uint8_t {
			return begin + index < end ? begin[index] : 0;
		}
	};
	return functions::peek(*this, bits, get_index);
}

void bitwise_readable_stream::advance_to_byte() noexcept {
	if (bit_index) {
		bit_index = 0;
		++byte_index;
	}
}

const std::uint8_t* bitwise_readable_stream::get_position() const noexcept {
	assert(!bit_index && "can\'t return a pointer to a bit, position should be at a byte boundary");
	return begin + byte_index;
}