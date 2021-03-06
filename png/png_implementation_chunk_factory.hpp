#pragma once
#include <cstdint>
#include <memory>
#include "png_implementation_image_data.hpp"
#include <span>
#include <vector>

using chunk_type_t = std::uint_fast32_t;

// data used by chunk_base::set_image_data
// used when reading an image from a string
struct image_construction_data {
	std::uint_fast32_t width, height;
	std::uint_fast8_t bit_depth, colour_type;
	bool uses_interlacing, pixel_data_is_set{ 0 };
	std::vector<truecolour_8> palette;
};

struct chunk_base {
	// 32 bit integer representing type
	// most significant byte is first byte in chunk type
	// least significant byte is fourth byte in chunk type
	virtual chunk_type_t get_type() const = 0;
	// function to set image data after private chunk data is set
	// set_image_data should throw if it is detected that there is an issue with other chunks
	virtual void set_image_data(image_construction_data& construction_data, image_data& out) = 0;
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
	chunk_type_t get_type() const override;
	void set_image_data(image_construction_data& construction_data, image_data& out) override;
	unknown_chunk(chunk_type_t chunk_type);
private:
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

// reads an 8 byte unsigned integer from in at position
// the most significant byte is first
// throws if can't read from that position
// advances position
// assumes position is at first element of in or beyond
std::uint_fast64_t read_8(const std::uint8_t*& position, const std::span<const std::uint8_t>& in);

// reads a 1 byte unsigned integer from in at position
// throws if can't read from that position
// advances position
// assumes position is at first element of in or beyond
std::uint_fast8_t read_1(const std::uint8_t*& position, const std::span<const std::uint8_t>& in);

// type of last chunk
constexpr chunk_type_t end_chunk_type{ 0x4945'4e44 };

// file header for png file
constexpr std::uint_fast64_t file_header{ 0x8950'4E47'0D0A'1A0A };

// creates chunks from in
// outputs chunks to out
// throws if there is an error reading
// terminates when end_chunk_type found
// throws if end_chunk_type not found
void create_chunks(std::vector<std::unique_ptr<chunk_base>>& out, std::span<const std::uint8_t> in);

// reads information from in,
// writes information to out
// throw if there is a problem
void set_image_data(image_data& out, std::vector<std::unique_ptr<chunk_base>>& in);