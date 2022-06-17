#pragma once
#include <vector>
#include <span>
#include <cstdint>
#include "utilities.h"

namespace png::deflate {
	// function that decompresses bits read from a bitwise_readable_stream called compressed and appends to std::vector<char>& out
	void inflate(std::vector<char>& out, bitwise_readable_stream& compressed);

	// function that compresses std::span<char> and appends to std::vector<std::uint8_t>& out
	void deflate(std::vector<std::uint8_t>& out, std::span<char> uncompressed);
}