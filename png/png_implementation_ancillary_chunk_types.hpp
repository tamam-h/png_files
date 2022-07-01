#include "png_implementation_chunk_factory.hpp"

std::unique_ptr<chunk_base> construct_gAMA(std::span<const std::uint8_t> in, const std::vector<std::unique_ptr<chunk_base>>& chunks);
// https://www.w3.org/TR/2003/REC-PNG-20031110/ section 11.3.3.2
struct gAMA_chunk : chunk_base {
	static constexpr chunk_type_t type{ 0x6741'4d41 };
	chunk_type_t get_type() const override;
	void set_image_data(image_construction_data& construction_data, image_data& out) override;
	gAMA_chunk(std::uint_fast32_t gamma);
	std::uint_fast32_t gamma;
};

// registers chunk types listed in this header file
void register_ancillary_chunk_types();