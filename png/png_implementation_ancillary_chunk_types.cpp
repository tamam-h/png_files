#include "pch.h"
#include "png_implementation_ancillary_chunk_types.hpp"
#include "png_implementation_chunk_factory.hpp"
#include "png_implementation_chunk_types.hpp"

gAMA_chunk::gAMA_chunk(std::uint_fast32_t gamma) : gamma{ gamma } {}

std::unique_ptr<chunk_base> construct_gAMA(std::span<const std::uint8_t> in, const std::vector<std::unique_ptr<chunk_base>>& chunks) {
	if (chunks.empty() || in.size() != 4) { goto fail; }
	for (const std::unique_ptr<chunk_base>& i : chunks) {
		if (i->get_type() == PLTE_chunk::type) { goto fail; }
		if (i->get_type() == IDAT_chunk::type) { goto fail; }
		if (i->get_type() == gAMA_chunk::type) { goto fail; }
	}
	{
		const std::uint8_t* position{ in.data() };
		return std::unique_ptr<chunk_base>{ new gAMA_chunk{ read_4(position, in) } };
	}
fail:
	return std::unique_ptr<chunk_base>{ new gAMA_chunk{ 0 } };
}

chunk_type_t gAMA_chunk::get_type() const {
	return type;
}

void gAMA_chunk::set_image_data(image_construction_data& construction_data, image_data& out) {
	out.colour_info.gamma = gamma;
}

void register_ancillary_chunk_types() {
	register_chunk_type(gAMA_chunk::type, construct_gAMA);
}