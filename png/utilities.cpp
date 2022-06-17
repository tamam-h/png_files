#include "pch.h"
#include "utilities.h"
#include <cstdint>
#include <span>
#include <cassert>
#include <concepts>

struct png::bitwise_readable_stream::data {
	const std::uint8_t* const begin, * const end;
	unsigned int byte_index{ 0 }, bit_index{ 0 };
	bool is_advanceable(unsigned int bits) const noexcept {
		unsigned int bit_index{ this->bit_index + bits };
		unsigned int byte_index{ this->byte_index + bit_index / 8 };
		bit_index %= 8;
		if (byte_index < end - begin) { return 1; }
		if (byte_index == end - begin) { return bit_index == 0; }
		return 0;
	}
	template <class functor_type> std::uint_fast16_t peek(unsigned int bits, const functor_type& access_function) const noexcept
		requires requires(unsigned int ui) {
			{ access_function(ui) } -> std::same_as<std::uint8_t>;
		} {
		std::uint_fast16_t acc{ 0 };
		unsigned int byte_index{ this->byte_index }, bit_index{ this->bit_index };
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

png::bitwise_readable_stream::bitwise_readable_stream(const std::uint8_t* begin, const std::uint8_t* end) : pointer_to_data{ new data{ begin, end } } {
	assert(pointer_to_data);
}

png::bitwise_readable_stream::operator std::span<const std::uint8_t>() const noexcept {
	return std::span<const std::uint8_t>{ pointer_to_data->begin, pointer_to_data->end };
}

std::uint_fast16_t png::bitwise_readable_stream::peek(unsigned int bits) const noexcept {
	assert(pointer_to_data->is_advanceable(bits));
	assert(bits <= 16);
	const auto get_index{
		[this](unsigned int index) -> std::uint8_t {
			return pointer_to_data->begin[index];
		}
	};
	return pointer_to_data->peek(bits, get_index);
}

void png::bitwise_readable_stream::advance(unsigned int bits) noexcept {
	assert(pointer_to_data->is_advanceable(bits));
	pointer_to_data->bit_index += bits;
	pointer_to_data->byte_index += pointer_to_data->bit_index / 8;
	pointer_to_data->bit_index %= 8;
}

bool png::bitwise_readable_stream::can_advance(unsigned int bits) const noexcept {
	return pointer_to_data->is_advanceable(bits);
}

std::uint_fast16_t png::bitwise_readable_stream::zero_extended_peek(unsigned int bits) const noexcept {
	assert(bits <= 16);
	if (pointer_to_data->is_advanceable(bits)) { return peek(bits); }
	const auto get_index{
		[this](unsigned int index) -> std::uint8_t {
			return pointer_to_data->begin + index < pointer_to_data->end ? pointer_to_data->begin[index] : 0;
		}
	};
	return pointer_to_data->peek(bits, get_index);
}

png::bitwise_readable_stream::~bitwise_readable_stream() {}