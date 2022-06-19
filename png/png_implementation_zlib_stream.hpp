#pragma once
#include <cstdint>
#include <vector>
#include <span>

// does not support preset dictionaries
struct zlib_stream_handler {
	// vectors to put uncompressed or compressed data
	std::vector<std::uint8_t> uncompressed_data, compressed_data;
	// compresses uncompressed_data into a zlib stream
	// clears and outputs to compressed_data
	// throws if there is a problem
	// leaves uncompressed_data unmodified
	void compress();
	// decompresses compressed_data to an array of std::uint8_t
	// clears and outputs to uncompressed_data
	// throws if there is a problem
	// leaves compressed_data unmodified
	void decompress();
};

// returns Adler-32 checksum of data in buffer
// https://www.rfc-editor.org/rfc/rfc1950.pdf section 9
std::uint_fast32_t adler32(std::span<const std::uint8_t> buffer);