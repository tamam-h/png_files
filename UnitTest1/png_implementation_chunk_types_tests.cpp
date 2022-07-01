#include "pch.h"
#include "png_implementation_chunk_factory.hpp"
#include "png_implementation_chunk_types.hpp"
#include "png_implementation_pixel_cast.hpp"
#include "png_implementation_zlib_stream.hpp"
#include <CppUnitTest.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace png_implementation_chunk_types_tests {
	bool operator==(const dimension_struct& first, const dimension_struct& second) {
		return first.width == second.width && first.height == second.height;
	}
	TEST_CLASS(construct_IHDR_tests) {
	public:
		TEST_METHOD(construct_IHDR_test_1) {
			std::vector<std::uint8_t> data(13);
			std::span<std::uint8_t> span{ data.data(), data.data() + data.size() };
			std::uint8_t* position{ data.data() };
			write(position, { 32, 234 });
			write(position, { 32, 424 });
			write(position, { 8, 16 });
			write(position, { 8, 4 });
			std::vector<std::unique_ptr<chunk_base>> vector;
			construct_IHDR(span, vector);
		}
		TEST_METHOD(construct_IHDR_test_2) {
			std::vector<std::uint8_t> data(13);
			std::span<std::uint8_t> span{ data.data(), data.data() + data.size() };
			std::uint8_t* position{ data.data() };
			bool should_fail{ 1 };
			write(position, { 32, 0 });
			write(position, { 32, 424 });
			write(position, { 8, 16 });
			write(position, { 8, 4 });
			std::vector<std::unique_ptr<chunk_base>> vector;
			try {
				construct_IHDR(span, vector);
			} catch (...) {
				should_fail = 0;
			}
			Assert::IsFalse(should_fail, L"can\'t have a zero dimension");
		}
		TEST_METHOD(construct_IHDR_test_3) {
			std::vector<std::uint8_t> data(13);
			std::span<std::uint8_t> span{ data.data(), data.data() + data.size() };
			std::uint8_t* position{ data.data() };
			bool should_fail{ 1 };
			write(position, { 32, 18 });
			write(position, { 32, 424 });
			write(position, { 8, 16 });
			write(position, { 8, 3 });
			std::vector<std::unique_ptr<chunk_base>> vector;
			try {
				construct_IHDR(span, vector);
			} catch (...) {
				should_fail = 0;
			}
			Assert::IsFalse(should_fail, L"16 bit depth is not allowed with colour type 3");
		}
		TEST_METHOD(construct_IHDR_test_4) {
			std::vector<std::uint8_t> data(13);
			std::span<std::uint8_t> span{ data.data(), data.data() + data.size() };
			std::uint8_t* position{ data.data() };
			bool should_fail{ 1 };
			write(position, { 32, 18 });
			write(position, { 32, 424 });
			write(position, { 8, 16 });
			write(position, { 8, 4 });
			write(position, { 8, 4 });
			std::vector<std::unique_ptr<chunk_base>> vector;
			try {
				construct_IHDR(span, vector);
			} catch (...) {
				should_fail = 0;
			}
			Assert::IsFalse(should_fail, L"compression method 4 is not allowed");
		}
		TEST_METHOD(construct_IHDR_test_5) {
			std::vector<std::uint8_t> data(13);
			std::span<std::uint8_t> span{ data.data(), data.data() + data.size() };
			std::uint8_t* position{ data.data() };
			bool should_fail{ 1 };
			write(position, { 32, 18 });
			write(position, { 32, 424 });
			write(position, { 8, 16 });
			write(position, { 8, 4 });
			write(position, { 8, 0 });
			write(position, { 8, 4 });
			std::vector<std::unique_ptr<chunk_base>> vector;
			try {
				construct_IHDR(span, vector);
			} catch (...) {
				should_fail = 0;
			}
			Assert::IsFalse(should_fail, L"filter method 4 is not allowed");
		}
		TEST_METHOD(construct_IHDR_test_6) {
			std::vector<std::uint8_t> data(13);
			std::span<std::uint8_t> span{ data.data(), data.data() + data.size() };
			std::uint8_t* position{ data.data() };
			bool should_fail{ 1 };
			write(position, { 32, 18 });
			write(position, { 32, 424 });
			write(position, { 8, 16 });
			write(position, { 8, 4 });
			write(position, { 8, 0 });
			write(position, { 8, 0 });
			write(position, { 8, 4 });
			std::vector<std::unique_ptr<chunk_base>> vector;
			try {
				construct_IHDR(span, vector);
			} catch (...) {
				should_fail = 0;
			}
			Assert::IsFalse(should_fail, L"interlace method 4 is not allowed");
		}
		TEST_METHOD(construct_IHDR_test_7) {
			std::vector<std::uint8_t> data(12);
			std::span<std::uint8_t> span{ data.data(), data.data() + data.size() };
			std::uint8_t* position{ data.data() };
			bool should_fail{ 1 };
			write(position, { 32, 499 });
			write(position, { 32, 424 });
			write(position, { 8, 16 });
			write(position, { 8, 4 });
			std::vector<std::unique_ptr<chunk_base>> vector;
			try {
				construct_IHDR(span, vector);
			} catch (...) {
				should_fail = 0;
			}
			Assert::IsFalse(should_fail, L"chunk is smaller than expected");
		}
		TEST_METHOD(construct_IHDR_test_8) {
			std::vector<std::uint8_t> data(14);
			std::span<std::uint8_t> span{ data.data(), data.data() + data.size() };
			std::uint8_t* position{ data.data() };
			bool should_fail{ 1 };
			write(position, { 32, 499 });
			write(position, { 32, 424 });
			write(position, { 8, 16 });
			write(position, { 8, 4 });
			std::vector<std::unique_ptr<chunk_base>> vector;
			try {
				construct_IHDR(span, vector);
			} catch (...) {
				should_fail = 0;
			}
			Assert::IsFalse(should_fail, L"chunk is larger than expected");
		}
		TEST_METHOD(construct_IHDR_test_9) {
			std::vector<std::uint8_t> data(13);
			std::span<std::uint8_t> span{ data.data(), data.data() + data.size() };
			std::uint8_t* position{ data.data() };
			write(position, { 32, 234 });
			write(position, { 32, 424 });
			write(position, { 8, 16 });
			write(position, { 8, 4 });
			std::vector<std::unique_ptr<chunk_base>> vector;
			vector.push_back(construct_IHDR(span, vector));
			bool should_fail{ 1 };
			try {
				construct_IHDR(span, vector);
			} catch (...) {
				should_fail = 0;
			}
			Assert::IsFalse(should_fail, L"there can\'t be two IHDR chunks");
		}
	};
	TEST_CLASS(construct_PLTE_tests) {
	public:
		TEST_METHOD(construct_PLTE_test_1) {
			std::vector<std::unique_ptr<chunk_base>> vector;
			vector.push_back(std::unique_ptr<chunk_base>{ new unknown_chunk{ IHDR_chunk::type } });
			std::vector<std::uint8_t> data(231);
			std::span<std::uint8_t> span{ data.data(), data.data() + data.size() };
			construct_PLTE(span, vector);
		}
		TEST_METHOD(construct_PLTE_test_2) {
			std::vector<std::unique_ptr<chunk_base>> vector;
			vector.push_back(std::unique_ptr<chunk_base>{ new unknown_chunk{ IHDR_chunk::type } });
			std::vector<std::uint8_t> data(1023);
			std::span<std::uint8_t> span{ data.data(), data.data() + data.size() };
			bool should_fail{ 1 };
			try {
				construct_PLTE(span, vector);
			} catch (...) {
				should_fail = 0;
			}
			Assert::IsFalse(should_fail, L"PLTE chunk has more entries than allowed");
		}
		TEST_METHOD(construct_PLTE_test_3) {
			std::vector<std::unique_ptr<chunk_base>> vector;
			vector.push_back(std::unique_ptr<chunk_base>{ new unknown_chunk{ IHDR_chunk::type } });
			std::vector<std::uint8_t> data(231);
			std::span<std::uint8_t> span{ data.data(), data.data() + data.size() };
			vector.push_back(construct_PLTE(span, vector));
			bool should_fail{ 1 };
			try {
				construct_PLTE(span, vector);
			} catch (...) {
				should_fail = 0;
			}
			Assert::IsFalse(should_fail, L"there can\'t be two PLTE chunks");
		}
		TEST_METHOD(construct_PLTE_test_4) {
			std::vector<std::unique_ptr<chunk_base>> vector;
			std::vector<std::uint8_t> data(231);
			std::span<std::uint8_t> span{ data.data(), data.data() + data.size() };
			bool should_fail{ 1 };
			try {
				construct_PLTE(span, vector);
			} catch (...) {
				should_fail = 0;
			}
			Assert::IsFalse(should_fail, L"there can\'t be a PLTE chunk if there is no IHDR chunk");
		}
		TEST_METHOD(construct_PLTE_test_5) {
			std::vector<std::unique_ptr<chunk_base>> vector;
			vector.push_back(std::unique_ptr<chunk_base>{ new unknown_chunk{ IHDR_chunk::type } });
			vector.push_back(std::unique_ptr<chunk_base>{ new unknown_chunk{ IDAT_chunk::type } });
			std::vector<std::uint8_t> data(231);
			std::span<std::uint8_t> span{ data.data(), data.data() + data.size() };
			bool should_fail{ 1 };
			try {
				construct_PLTE(span, vector);
			} catch (...) {
				should_fail = 0;
			}
			Assert::IsFalse(should_fail, L"PLTE chunk can\'t come after IDAT chunks");
		}
		TEST_METHOD(construct_PLTE_test_6) {
			std::vector<std::unique_ptr<chunk_base>> vector;
			vector.push_back(std::unique_ptr<chunk_base>{ new unknown_chunk{ IHDR_chunk::type } });
			std::vector<std::uint8_t> data(230);
			std::span<std::uint8_t> span{ data.data(), data.data() + data.size() };
			bool should_fail{ 1 };
			try {
				construct_PLTE(span, vector);
			} catch (...) {
				should_fail = 0;
			}
			Assert::IsFalse(should_fail, L"size of data in PLTE chunk should be a multiple of 3");
		}
	};
	TEST_CLASS(construct_IDAT_tests) {
	public:
		TEST_METHOD(construct_IDAT_test_1) {
			std::vector<std::unique_ptr<chunk_base>> vector;
			vector.push_back(std::unique_ptr<chunk_base>{ new unknown_chunk{ IHDR_chunk::type } });
			zlib_stream_handler curr; curr.uncompressed_data.resize(363);
			curr.compress();
			std::vector<std::uint8_t> data{ curr.compressed_data };
			std::span<std::uint8_t> span{ data.data(), data.data() + data.size() };
			construct_IDAT(span, vector);
		}
		TEST_METHOD(construct_IDAT_test_2) {
			std::vector<std::unique_ptr<chunk_base>> vector;
			vector.push_back(std::unique_ptr<chunk_base>{ new unknown_chunk{ IHDR_chunk::type } });
			vector.push_back(std::unique_ptr<chunk_base>{ new unknown_chunk{ PLTE_chunk::type } });
			zlib_stream_handler curr; curr.uncompressed_data.resize(363);
			curr.compress();
			std::vector<std::uint8_t> data{ curr.compressed_data };
			std::span<std::uint8_t> span{ data.data(), data.data() + data.size() - 1 };
			vector.push_back(construct_IDAT(span, vector));
			construct_IDAT({ data.data() + data.size() - 1, data.data() + data.size() }, vector);
		}
		TEST_METHOD(construct_IDAT_test_3) {
			std::vector<std::unique_ptr<chunk_base>> vector;
			vector.push_back(std::unique_ptr<chunk_base>{ new unknown_chunk{ IHDR_chunk::type } });
			vector.push_back(std::unique_ptr<chunk_base>{ new unknown_chunk{ PLTE_chunk::type } });
			zlib_stream_handler curr; curr.uncompressed_data.resize(363);
			curr.compress();
			std::vector<std::uint8_t> data{ curr.compressed_data };
			std::span<std::uint8_t> span{ data.data(), data.data() + data.size() - 1 };
			vector.push_back(construct_IDAT(span, vector));
			vector.push_back(std::unique_ptr<chunk_base>{ new unknown_chunk{ 'A' << 24 | 'A' << 16 | 'A' << 8 | 'A' } });
			bool should_fail{ 1 };
			try {
				construct_IDAT({ data.data() + data.size() - 1, data.data() + data.size() }, vector);
			} catch (...) {
				should_fail = 0;
			}
			Assert::IsFalse(should_fail, L"if there are already IDAT chunks in the stream, they must be at the end of the vector");
		}
		TEST_METHOD(construct_IDAT_test_4) {
			std::vector<std::unique_ptr<chunk_base>> vector;
			zlib_stream_handler curr; curr.uncompressed_data.resize(363);
			curr.compress();
			std::vector<std::uint8_t> data{ curr.compressed_data };
			std::span<std::uint8_t> span{ data.data(), data.data() + data.size() - 1 };
			bool should_fail{ 1 };
			try {
				construct_IDAT({ data.data() + data.size() - 1, data.data() + data.size() }, vector);
			} catch (...) {
				should_fail = 0;
			}
			Assert::IsFalse(should_fail, L"there should be an IHDR chunk before an IDAT chunk");
		}
	};
	TEST_CLASS(construct_IEND_tests) {
	public:
		TEST_METHOD(construct_IEND_test_1) {
			std::vector<std::unique_ptr<chunk_base>> vector;
			vector.push_back(std::unique_ptr<chunk_base>{ new unknown_chunk{ IHDR_chunk::type } });
			vector.push_back(std::unique_ptr<chunk_base>{ new unknown_chunk{ IDAT_chunk::type } });
			std::vector<std::uint8_t> data(1);
			construct_IEND({ data.data(), data.data() }, vector);
		}
		TEST_METHOD(construct_IEND_test_2) {
			std::vector<std::unique_ptr<chunk_base>> vector;
			vector.push_back(std::unique_ptr<chunk_base>{ new unknown_chunk{ IHDR_chunk::type } });
			vector.push_back(std::unique_ptr<chunk_base>{ new unknown_chunk{ IDAT_chunk::type } });
			std::vector<std::uint8_t> data(1);
			bool should_fail{ 1 };
			try {
				construct_IEND({ data.data(), data.data() + 1 }, vector);
			} catch (...) {
				should_fail = 0;
			}
			Assert::IsFalse(should_fail, L"IEND chunks should have a size of zero");
		}
		TEST_METHOD(construct_IEND_test_3) {
			std::vector<std::unique_ptr<chunk_base>> vector;
			vector.push_back(std::unique_ptr<chunk_base>{ new unknown_chunk{ IHDR_chunk::type } });
			std::vector<std::uint8_t> data(1);
			bool should_fail{ 1 };
			try {
				construct_IEND({ data.data(), data.data() }, vector);
			} catch (...) {
				should_fail = 0;
			}
			Assert::IsFalse(should_fail, L"there should be an IDAT chunk in the vector");
		}
	};
	TEST_CLASS(interlaced_dimensions_tests) {
	public:
		TEST_METHOD(interlaced_dimensions_test_1) {
			dimension_struct input{ 8, 8 };
			Assert::IsTrue(dimension_struct{ 1, 1 } == interlaced_dimensions(0, input));
		}
		TEST_METHOD(interlaced_dimensions_test_2) {
			dimension_struct input{ 4, 4 };
			Assert::IsTrue(dimension_struct{ 0, 0 } == interlaced_dimensions(2, input));
		}
		TEST_METHOD(interlaced_dimensions_test_3) {
			dimension_struct input{ 1000, 1 };
			Assert::IsTrue(dimension_struct{ 0, 0 } == interlaced_dimensions(6, input));
		}
		TEST_METHOD(interlaced_dimensions_test_4) {
			dimension_struct input{ 9, 2 };
			Assert::IsTrue(dimension_struct{ 9, 1 } == interlaced_dimensions(6, input));
		}
		TEST_METHOD(interlaced_dimensions_test_5) {
			dimension_struct input{ 9, 9 };
			Assert::IsTrue(dimension_struct{ 9, 4 } == interlaced_dimensions(6, input));
		}
		TEST_METHOD(interlaced_dimensions_test_6) {
			dimension_struct input{ 8, 10 };
			Assert::IsTrue(dimension_struct{ 8, 5 } == interlaced_dimensions(6, input));
		}
		TEST_METHOD(interlaced_dimensions_test_7) {
			dimension_struct input{ 7, 10 };
			Assert::IsTrue(dimension_struct{ 7, 5 } == interlaced_dimensions(6, input));
		}
		TEST_METHOD(interlaced_dimensions_test_8) {
			dimension_struct input{ 9, 10 };
			Assert::IsTrue(dimension_struct{ 9, 5 } == interlaced_dimensions(6, input));
		}
		TEST_METHOD(interlaced_dimensions_test_9) {
			dimension_struct input{ 7, 7 };
			Assert::IsTrue(dimension_struct{ 2, 2 } == interlaced_dimensions(3, input));
		}
		TEST_METHOD(interlaced_dimensions_test_10) {
			dimension_struct input{ 6, 7 };
			Assert::IsTrue(dimension_struct{ 1, 2 } == interlaced_dimensions(3, input));
		}
		TEST_METHOD(interlaced_dimensions_test_11) {
			dimension_struct input{ 80, 80 };
			Assert::IsTrue(dimension_struct{ 10, 10 } == interlaced_dimensions(0, input));
		}
		TEST_METHOD(interlaced_dimensions_test_12) {
			dimension_struct input{ 81, 81 };
			Assert::IsTrue(dimension_struct{ 11, 11 } == interlaced_dimensions(0, input));
		}
		TEST_METHOD(interlaced_dimensions_test_13) {
			dimension_struct input{ 1, 80 };
			Assert::IsTrue(dimension_struct{ 1, 10 } == interlaced_dimensions(0, input));
		}
		TEST_METHOD(interlaced_dimensions_test_14) {
			dimension_struct input{ 1, 81 };
			Assert::IsTrue(dimension_struct{ 1, 11 } == interlaced_dimensions(0, input));
		}
		TEST_METHOD(interlaced_dimensions_test_15) {
			dimension_struct input{ 2, 1 };
			Assert::IsTrue(dimension_struct{ 1, 1 } == interlaced_dimensions(5, input));
		}
	};
	TEST_CLASS(get_pixel_tests) {
	public:
		TEST_METHOD(get_pixel_test_1) {
			std::vector<std::vector<std::uint8_t>> reduced_image{ { 0, 0b1000'0000 } };
			Assert::IsTrue(get_pixel(reduced_image, 0, 0, 0b0001) == 0b1);
		}
		TEST_METHOD(get_pixel_test_2) {
			std::vector<std::vector<std::uint8_t>> reduced_image{ { 0, 0b0100'0000 } };
			Assert::IsTrue(get_pixel(reduced_image, 0, 1, 0b0001) == 0b1);
		}
		TEST_METHOD(get_pixel_test_3) {
			std::vector<std::vector<std::uint8_t>> reduced_image{ { 0, 0b1000'0000 } };
			Assert::IsTrue(get_pixel(reduced_image, 0, 0, 0b0010) == 0b10);
		}
		TEST_METHOD(get_pixel_test_4) {
			std::vector<std::vector<std::uint8_t>> reduced_image{ { 0, 0b0000'0111 } };
			Assert::IsTrue(get_pixel(reduced_image, 0, 1, 0b0100) == 0b0111);
		}
		TEST_METHOD(get_pixel_test_5) {
			std::vector<std::vector<std::uint8_t>> reduced_image{ { 0, 0b0000'0000, 0b0000'0111 } };
			Assert::IsTrue(get_pixel(reduced_image, 0, 3, 0b0100) == 0b0111);
		}
		TEST_METHOD(get_pixel_test_6) {
			std::vector<std::vector<std::uint8_t>> reduced_image{ { 0, 0b0000'0000, 0b0010'0111 } };
			Assert::IsTrue(get_pixel(reduced_image, 0, 1, 0b1000) == 0b0010'0111);
		}
		TEST_METHOD(get_pixel_test_7) {
			std::vector<std::vector<std::uint8_t>> reduced_image{ { 0, 0b0000'0000, 0b1111'0111 }, { 0, 0b0000'0000, 0b0010'0111 } };
			Assert::IsTrue(get_pixel(reduced_image, 1, 1, 0b1000) == 0b0010'0111);
		}
		TEST_METHOD(get_pixel_test_8) {
			std::vector<std::vector<std::uint8_t>> reduced_image{ { 0, 0b0100'0010, 0b1111'0111 }, { 0, 0b0000'0000, 0b0010'0111 } };
			Assert::IsTrue(get_pixel(reduced_image, 0, 0, 0b10000) == 0b0100'0010'1111'0111);
		}
	};
}