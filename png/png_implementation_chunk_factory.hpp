#pragma once
#include <cstdint>
#include <memory>
#include "png_implementation_image_data.hpp"
#include <span>
#include <vector>

using chunk_type_t = std::uint_fast32_t;

struct chunk_base {
	// 32 bit integer representing type
	// most significant byte is first byte in chunk type
	// least significant byte is fourth byte in chunk type
	virtual chunk_type_t get_type() = 0;
	// function to set image data after private chunk data is set
	// chunks is a vector of all chunks that will be used to set image data
	// index is the index of this chunk
	virtual void set_image_data(const std::vector<std::unique_ptr<chunk_base>>& chunks, unsigned int index, image_data& out) = 0;
	// function to write chuck data to out
	// function should not write type or CRC-32 value
	virtual void write_chunk_data(std::vector<std::uint8_t>& out) = 0;
	// returns the number of bytes that will be written to out if write_chunk_data is called
	virtual std::uint_fast32_t get_chunk_size() = 0;
	virtual ~chunk_base();
};

// returns if the chunk type is a valid chunk type
// all bytes are between a-z or A-Z inclusive
// https://www.w3.org/TR/2003/REC-PNG-20031110/ section 5.3
bool is_valid_chunk_type(chunk_type_t in);

// returns if the chunk type is a critical chunk type
// https://www.w3.org/TR/2003/REC-PNG-20031110/ section 5.4
// assumes is valid chunk type
bool is_critical_chunk_type(chunk_type_t in);

// returns if bit 5 is set in third byte
// https://www.w3.org/TR/2003/REC-PNG-20031110/ section 5.4
// assumes is valid chunk type
bool reserved_bit_set(chunk_type_t in);

// create_chunk points to a function that creates a chunk from std::span<const std::uint8_t> in
// in will not contain chunk type or CRC-32
// chunks is a vector of all previous created chunks for the image data being created
// create_chunk should throw if it is detected that the chunks are not in the correct order
// in will not exist for the lifetime of a chunk
// assumes is valid chunk type
// assumes reserved bit is not set
void register_chunk_type(chunk_type_t type, std::unique_ptr<chunk_base> (*create_chunk)(std::span<const std::uint8_t> in, const std::vector<std::unique_ptr<chunk_base>>& chunks));

// unregisters all chunk types
void clear_chunk_type_registry();

// this chunk type is special and should not be registered
struct unknown_chunk : chunk_base {
	chunk_type_t get_type() override;
	void set_image_data(const std::vector<std::unique_ptr<chunk_base>>& chunks, unsigned int index, image_data& out) override;
	void write_chunk_data(std::vector<std::uint8_t>& out) override;
	std::uint_fast32_t get_chunk_size() override;
	unknown_chunk(std::span<const std::uint8_t> content, chunk_type_t chunk_type);
private:
	const std::vector<std::uint8_t> content;
	const chunk_type_t chunk_type;
};

// function that returns the CRC-32 of content pointed to by in
std::uint_fast32_t crc32(std::span<const std::uint8_t> in);

// does nothing if it safe to read from position
// throws if it is unsafe to read from position
// assumes position is at first element of in or beyond
void assert_can_read(const std::uint8_t* position, const std::span<const std::uint8_t>& in);

// reads a 4 byte unsigned integer from in at position
// the most significant byte is first
// throws if can't read from that position
// advances position
// assumes position is at first element of in or beyond
std::uint_fast32_t read_4(const std::uint8_t*& position, const std::span<const std::uint8_t>& in);

// writes a 4 byte unsigned integer to position[0] through position[3] and then advances position
// the most significant byte is first
// throws if can't write to that position
// assumes position is at first element of out or beyond
void write_4(std::uint_fast32_t value, std::uint8_t*& position, const std::span<std::uint8_t>& out);

// reads an 8 byte unsigned integer from in at position
// the most significant byte is first
// throws if can't read from that position
// advances position
// assumes position is at first element of in or beyond
std::uint_fast64_t read_8(const std::uint8_t*& position, const std::span<const std::uint8_t>& in);

// writes an 8 byte unsigned integer to position[0] through position[7] and then advances position
// the most significant byte is first
// throws if can't write to that position
// assumes position is at first element of out or beyond
void write_8(std::uint_fast64_t value, std::uint8_t*& position, const std::span<std::uint8_t>& out);

// constant that specifies the chunk type of IEND
constexpr chunk_type_t end_chunk_type{ static_cast<chunk_type_t>(73) << 24
	| static_cast<chunk_type_t>(69) << 16
	| static_cast<chunk_type_t>(78) << 8
	| static_cast<chunk_type_t>(68) };

// creates chunks from in
// outputs chunks to out
// throws if there is an error reading
// throws if a chunk of end_chunk_type is not at end
void create_chunks(std::vector<std::unique_ptr<chunk_base>>& out, std::span<const std::uint8_t> in);