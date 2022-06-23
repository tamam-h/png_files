#include "pch.h"
#include "png_implementation_zlib_stream.hpp"
#include "png_implementation_deflate_algorithm.hpp"

std::uint_fast32_t adler32(std::span<const std::uint8_t> buffer) {
	// https://www.rfc-editor.org/rfc/rfc1950.pdf section 9
	const std::uint_fast32_t mod{ 65521 };
	std::uint_fast32_t s1{ 1 }, s2{ 0 };
	for (const std::uint8_t i : buffer) {
		s1 += i;
		if (s1 > mod) { s1 -= mod; }
		s2 += s1;
		if (s2 > mod) { s2 -= mod; }
	}
	return s2 << 16 | s1;
}

void zlib_stream_handler::compress() {
	// https://www.rfc-editor.org/rfc/rfc1950.pdf section 2.2
	compressed_data.clear();
	compressed_data.reserve(2);
	compressed_data.push_back(0b0111'1000);
	compressed_data.push_back(0b0000'0001);
	std::span<const std::uint8_t> span{ uncompressed_data.data(), uncompressed_data.data() + uncompressed_data.size() };
	::compress(compressed_data, span);
	std::uint_fast32_t checksum{ adler32(span) };
	compressed_data.reserve(compressed_data.size() + 4);
	compressed_data.push_back((checksum & 0xFF00'0000) >> 24);
	compressed_data.push_back((checksum & 0x00FF'0000) >> 16);
	compressed_data.push_back((checksum & 0x0000'FF00) >> 8);
	compressed_data.push_back(checksum & 0x0000'00FF);
}

void zlib_stream_handler::decompress() {
	// https://www.rfc-editor.org/rfc/rfc1950.pdf section 2.2
	uncompressed_data.clear();
	if (compressed_data.size() < 6) { throw std::runtime_error{ "zlib stream should have a size that is six or greater" }; }
	std::uint_fast8_t first_byte{ compressed_data[0] };
	if ((first_byte & 0xF) != 8) { throw std::runtime_error{ "zlib stream with compression method other than 8 is unimplemented" }; }
	std::uint_fast8_t second_byte{ compressed_data[1] };
	if ((static_cast<std::uint_fast16_t>(first_byte << 8) | second_byte) % 31) { throw std::runtime_error{ "first two bytes of zlib stream read as a 16 bit integer with most significant byte first should be a multiple of 31" }; }
	if (second_byte & 0b10'0000) { throw std::runtime_error{ "use of preset dictionary in zlib stream is unimplemented" }; }
	bitwise_readable_stream stream{ { compressed_data.data() + 2, compressed_data.data() + compressed_data.size() - 4 } };
	::decompress(uncompressed_data, stream);
	std::uint_fast32_t checksum{ 0 };
	checksum |= compressed_data.rbegin()[0];
	checksum |= compressed_data.rbegin()[1] << 8;
	checksum |= compressed_data.rbegin()[2] << 16;
	checksum |= compressed_data.rbegin()[3] << 24;
	if (adler32({ uncompressed_data.data(), uncompressed_data.data() + uncompressed_data.size() }) != checksum) { throw std::runtime_error{ "Adler-32 value does not match calculated Adler-32" }; }
}