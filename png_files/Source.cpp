#include "png.hpp"
#include <cstdint>
#include <fstream>
#include <vector>
#include <cstdlib>
int main() {
	std::vector<std::uint8_t> buffer;
	{
		std::basic_filebuf<std::uint8_t> curr; curr.open("output.png", std::ios::in | std::ios::binary);
		while (curr.sgetc() != std::char_traits<std::uint8_t>::eof()) {
			buffer.push_back(curr.sbumpc());
		}
	}
	png input{ { buffer.data(), buffer.data() + buffer.size() } };
	std::vector<std::vector<truecolour_8>>& arr{ input.get_array<truecolour_8>() };
	for (std::vector<truecolour_8>& i : arr) {
		for (truecolour_8& j : i) {
			j.red = std::rand() % 256;
			j.green = std::rand() % 256;
			j.blue = std::rand() % 256;
		}
	}
	buffer.clear();
	input.get_gamma() = 25000;
	input.write_to(buffer);
	{
		std::basic_filebuf<std::uint8_t> curr; curr.open("output.png", std::ios::out | std::ios::binary);
		for (auto i : buffer) {
			curr.sputc(i);
		}
	}
	return 0;
}