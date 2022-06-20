#pragma once
#include <cstdint>
#include <map>
#include <memory>
#include "png_implementation_image_data.hpp"
#include <span>
#include <vector>

using chunk_type_t = std::uint_fast32_t;

struct chunk_base {
	// 32 bit integer representing type
	// least significant byte is first byte in chunk type
	// most significant byte is fourth byte in chunk type
	virtual chunk_type_t get_type() = 0;
	// function to set image data after private chunk data is set
	// chunks is a vector of all chunks that will be used to set image data
	// index is the index of this chunk
	virtual void set_image_data(const std::vector<std::unique_ptr<chunk_base>>& chunks, unsigned int index, image_data& out) = 0;
	// function to write chuck data to out
	// function should not write type or CRC32 value
	virtual void write_chunk_data(std::vector<std::uint8_t>& out) = 0;
	// returns the number of bytes that will be written if write_chunk_data is called
	virtual std::uint_fast32_t get_chunk_size() = 0;
	virtual ~chunk_base();
};

// create_chunk points to a function that creates a chunk from std::span<const std::uint8_t> in
// in will not contain chunk type or CRC32 value
// chunks is a vector of all previous created chunks for the image data being created
// create_chunk should throw if it is detected that the chunks are not in the correct order
// in will not exist for the lifetime of a chunk
void register_chunk_type(chunk_type_t type, std::unique_ptr<chunk_base> (*create_chunk)(std::span<const std::uint8_t> in, const std::vector<std::unique_ptr<chunk_base>>& chunks));

// unregisters all chunk types
// registers IHDR, PLTE, IDAT, IEND, and tRNS chunk types
void reset_chunk_registry();

// creates chunks from in
// outputs chunks to out
// throws if there is an error reading
void create_chunks(std::vector<std::unique_ptr<chunk_base>>& out, std::span<const std::uint8_t> in);

struct unknown_chunk : chunk_base {

};

struct trailing_chunk : chunk_base {

};

std::unique_ptr<chunk_base> add_IHDR(std::span<const std::uint8_t> in, const std::vector<std::unique_ptr<chunk_base>>& chunks);
struct IHDR_chunk : chunk_base {

};

std::unique_ptr<chunk_base> add_PLTE(std::span<const std::uint8_t> in, const std::vector<std::unique_ptr<chunk_base>>& chunks);
struct PLTE_chunk : chunk_base {

};

std::unique_ptr<chunk_base> add_IDAT(std::span<const std::uint8_t> in, const std::vector<std::unique_ptr<chunk_base>>& chunks);
struct IDAT_chunk : chunk_base {

};

std::unique_ptr<chunk_base> add_IEND(std::span<const std::uint8_t> in, const std::vector<std::unique_ptr<chunk_base>>& chunks);
struct IEND_chunk : chunk_base {

};

std::unique_ptr<chunk_base> add_tRNS(std::span<const std::uint8_t> in, const std::vector<std::unique_ptr<chunk_base>>& chunks);
struct tRNS_chunk : chunk_base {

};