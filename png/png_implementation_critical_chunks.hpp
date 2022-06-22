#pragma once
#include "png_implementation_chunk_factory.hpp"

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