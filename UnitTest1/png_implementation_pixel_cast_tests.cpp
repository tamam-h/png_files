#include "pch.h"
#include "png_implementation_chunk_factory.hpp"
#include "png_implementation_pixel_cast.hpp"
#include "png_implementation_pixel_types.hpp"
#include <CppUnitTest.h>
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace pixel_cast_tests {
	bool operator==(const integral_pixel_info& first, const integral_pixel_info& second) {
		return first.bytes == second.bytes && first.value == second.value;
	}
	TEST_CLASS(pixel_cast_tests) {
	public:
		TEST_METHOD(pixel_cast_test_1) {
			greyscale_1 first{ 1 };
			Assert::IsTrue(static_cast<std::uint8_t>(3) == pixel_cast<greyscale_2>(first).grey);
		}
		TEST_METHOD(pixel_cast_test_2) {
			greyscale_1 first{ 1 };
			Assert::IsTrue(static_cast<std::uint8_t>(15) == pixel_cast<greyscale_4>(first).grey);
		}
		TEST_METHOD(pixel_cast_test_3) {
			greyscale_2 first{ 1 };
			Assert::IsTrue(static_cast<std::uint8_t>(1) == pixel_cast<greyscale_2>(first).grey);
		}
		TEST_METHOD(pixel_cast_test_4) {
			greyscale_8 first{ 37 };
			greyscale_with_alpha_8 casted{ pixel_cast<greyscale_with_alpha_8>(first) };
			Assert::IsTrue(static_cast<std::uint8_t>(37) == casted.grey);
			Assert::IsTrue(static_cast<std::uint8_t>(255) == casted.alpha);
		}
		TEST_METHOD(pixel_cast_test_5) {
			greyscale_16 first{ 235 };
			greyscale_16 casted{ pixel_cast<greyscale_16>(first) };
			Assert::IsTrue(first.grey == casted.grey);
		}
		TEST_METHOD(pixel_cast_test_6) {
			greyscale_16 first{ 234 };
			truecolour_16 casted{ pixel_cast<truecolour_16>(first) };
			Assert::IsTrue(first.grey == casted.red);
			Assert::IsTrue(first.grey == casted.green);
			Assert::IsTrue(first.grey == casted.blue);
		}
		TEST_METHOD(pixel_cast_test_7) {
			greyscale_float first{ 25.0 / 255.0 };
			greyscale_with_alpha_8 casted{ pixel_cast<greyscale_with_alpha_8>(first) };
			Assert::IsTrue(static_cast<std::uint8_t>(255) == casted.alpha);
			Assert::IsTrue(static_cast<std::uint8_t>(25) == casted.grey);
		}
		TEST_METHOD(pixel_cast_test_8) {
			truecolour_float first{ 1, 58.0 / 255.0, 234.0 / 255.0 };
			truecolour_with_alpha_8 casted{ pixel_cast<truecolour_with_alpha_8>(first) };
			Assert::IsTrue(static_cast<std::uint8_t>(255) == casted.red);
			Assert::IsTrue(static_cast<std::uint8_t>(58) == casted.green);
			Assert::IsTrue(static_cast<std::uint8_t>(234) == casted.blue);
			Assert::IsTrue(static_cast<std::uint8_t>(255) == casted.alpha);
		}
		TEST_METHOD(pixel_cast_test_9) {
			truecolour_8 first{ 0, 4, 250 };
			greyscale_2 casted{ pixel_cast<greyscale_2>(first) };
			Assert::IsTrue(static_cast<std::uint8_t>(3) == casted.grey);
		}
		TEST_METHOD(pixel_cast_test_10) {
			truecolour_8 first{ 145, 4, 168 };
			greyscale_2 casted{ pixel_cast<greyscale_2>(first) };
			Assert::IsTrue(static_cast<std::uint8_t>(2) == casted.grey);
		}
		TEST_METHOD(pixel_cast_test_11) {
			truecolour_8 first{ 80, 4, 80 };
			greyscale_2 casted{ pixel_cast<greyscale_2>(first) };
			Assert::IsTrue(static_cast<std::uint8_t>(1) == casted.grey);
		}
		TEST_METHOD(pixel_cast_test_12) {
			truecolour_8 first{ 0, 4, 3 };
			greyscale_4 casted{ pixel_cast<greyscale_4>(first) };
			Assert::IsTrue(static_cast<std::uint8_t>(0) == casted.grey);
		}
		TEST_METHOD(pixel_cast_test_13) {
			truecolour_with_alpha_8 curr{ 123, 234, 11, 8 };
			truecolour_8 casted{ pixel_cast<truecolour_8>(curr) };
			Assert::IsTrue(curr.red == casted.red);
			Assert::IsTrue(curr.green == casted.green);
			Assert::IsTrue(curr.blue == casted.blue);
		}
		TEST_METHOD(pixel_cast_test_14) {
			truecolour_16 first{ 37, 65535, 35 };
			greyscale_with_alpha_8 casted{ pixel_cast<greyscale_with_alpha_8>(first) };
			Assert::IsTrue(static_cast<std::uint8_t>(255) == casted.grey);
			Assert::IsTrue(static_cast<std::uint8_t>(255) == casted.alpha);
		}
	};
	TEST_CLASS(to_pixel_tests) {
	public:
		TEST_METHOD(to_pixel_test_1) {
			Assert::IsTrue(to_pixel<greyscale_1>(0, {}, 0).grey == 0);
		}
		TEST_METHOD(to_pixel_test_2) {
			Assert::IsTrue(to_pixel<greyscale_1>(1, {}, 0).grey == 1);
		}
		TEST_METHOD(to_pixel_test_3) {
			Assert::IsTrue(to_pixel<greyscale_2>(0, {}, 0).grey == 0);
		}
		TEST_METHOD(to_pixel_test_4) {
			Assert::IsTrue(to_pixel<greyscale_2>(3, {}, 0).grey == 3);
		}
		TEST_METHOD(to_pixel_test_5) {
			Assert::IsTrue(to_pixel<greyscale_8>(0b1101'0011, {}, 0).grey == 0b1101'0011);
		}
		TEST_METHOD(to_pixel_test_6) {
			Assert::IsTrue(to_pixel<greyscale_16>(0b1101'0011'1111'0001, {}, 0).grey == 0b1101'0011'1111'0001);
		}
		TEST_METHOD(to_pixel_test_7) {
			truecolour_8 returned{ to_pixel<truecolour_8>(0b1100'0001'1101'0011'1111'0001, {}, 0) };
			Assert::IsTrue(returned.red == 0b1100'0001);
			Assert::IsTrue(returned.green == 0b1101'0011);
			Assert::IsTrue(returned.blue == 0b1111'0001);
		}
		TEST_METHOD(to_pixel_test_8) {
			truecolour_16 returned{ to_pixel<truecolour_16>(0b1100'0001'1101'0011'1111'0001'1100'0001'1101'0011'1111'0001, {}, 0) };
			Assert::IsTrue(returned.red == 0b1100'0001'1101'0011);
			Assert::IsTrue(returned.green == 0b1111'0001'1100'0001);
			Assert::IsTrue(returned.blue == 0b1101'0011'1111'0001);
		}
		TEST_METHOD(to_pixel_test_9) {
			truecolour_8 returned{ to_pixel<truecolour_8>(2, { {}, {}, { 78, 96, 22 }, {} }, 1) };
			Assert::IsTrue(returned.red == 78);
			Assert::IsTrue(returned.green == 96);
			Assert::IsTrue(returned.blue = 22);
		}
		TEST_METHOD(to_pixel_test_10) {
			bool should_fail{ 1 };
			try {
				to_pixel<truecolour_8>(4, { {}, {}, {}, {} }, 1);
			} catch (...) {
				should_fail = 0;
			}
			Assert::IsFalse(should_fail);
		}
		TEST_METHOD(to_pixel_test_11) {
			truecolour_with_alpha_8 returned{ to_pixel<truecolour_with_alpha_8>(0b1100'0001'1101'0011'1111'0001'0011'1100, {}, 0) };
			Assert::IsTrue(returned.red == 0b1100'0001);
			Assert::IsTrue(returned.green == 0b1101'0011);
			Assert::IsTrue(returned.blue == 0b1111'0001);
			Assert::IsTrue(returned.alpha == 0b0011'1100);
		}
		TEST_METHOD(to_pixel_test_12) {
			truecolour_with_alpha_16 returned{ to_pixel<truecolour_with_alpha_16>(0b1100'0001'1101'0011'1111'0001'0011'1100'1100'0001'1101'0000'1111'0001'1111'1100, {}, 0) };
			Assert::IsTrue(returned.red == 0b1100'0001'1101'0011);
			Assert::IsTrue(returned.green == 0b1111'0001'0011'1100);
			Assert::IsTrue(returned.blue == 0b1100'0001'1101'0000);
			Assert::IsTrue(returned.alpha == 0b1111'0001'1111'1100);
		}
		TEST_METHOD(to_pixel_test_13) {
			greyscale_with_alpha_8 returned{ to_pixel<greyscale_with_alpha_8>(0b1110'1001'0001'0011, {}, 0) };
			Assert::IsTrue(returned.grey == 0b1110'1001);
			Assert::IsTrue(returned.alpha == 0b0001'0011);
		}
		TEST_METHOD(to_pixel_test_14) {
			greyscale_with_alpha_16 returned{ to_pixel<greyscale_with_alpha_16>(0b1110'1001'0001'0011'1110'1001'0001'0011, {}, 0) };
			Assert::IsTrue(returned.grey == 0b1110'1001'0001'0011);
			Assert::IsTrue(returned.alpha == 0b1110'1001'0001'0011);
		}
	};
	TEST_CLASS(to_integral_pixel_tests) {
	public:
		TEST_METHOD(to_integral_pixel_test_1) {
			Assert::IsTrue(integral_pixel_info{ 0b1, 0 } == to_integral_pixel(greyscale_1{ 0 }));
		}
		TEST_METHOD(to_integral_pixel_test_2) {
			Assert::IsTrue(integral_pixel_info{ 0b1, 1 } == to_integral_pixel(greyscale_1{ 1 }));
		}
		TEST_METHOD(to_integral_pixel_test_3) {
			Assert::IsTrue(integral_pixel_info{ 0b10, 0b11 } == to_integral_pixel(greyscale_2{ 0b11 }));
		}
		TEST_METHOD(to_integral_pixel_test_4) {
			Assert::IsTrue(integral_pixel_info{ 0b100, 0b1010 } == to_integral_pixel(greyscale_4{ 0b1010 }));
		}
		TEST_METHOD(to_integral_pixel_test_5) {
			Assert::IsTrue(integral_pixel_info{ 0b1000, 0b1100'1010 } == to_integral_pixel(greyscale_8{ 0b1100'1010 }));
		}
		TEST_METHOD(to_integral_pixel_test_6) {
			Assert::IsTrue(integral_pixel_info{ 0b1'0000, 0b1100'1010'1111'1000 } == to_integral_pixel(greyscale_16{ 0b1100'1010'1111'1000 }));
		}
		TEST_METHOD(to_integral_pixel_test_7) {
			Assert::IsTrue(integral_pixel_info{ 0b1'1000, 0b1100'1010'1111'1010'1000'0001 } == to_integral_pixel(truecolour_8{ 0b1100'1010, 0b1111'1010, 0b1000'0001 }));
		}
		TEST_METHOD(to_integral_pixel_test_8) {
			Assert::IsTrue(integral_pixel_info{ 0b11'0000, 0b1100'1010'0011'1110'1111'1010'0000'1110'1000'0001'0011'0000 } == to_integral_pixel(truecolour_16{ 0b1100'1010'0011'1110, 0b1111'1010'0000'1110, 0b1000'0001'0011'0000 }));
		}
		TEST_METHOD(to_integral_pixel_test_9) {
			Assert::IsTrue(integral_pixel_info{ 0b1'0000, 0b1100'1010'0001'1110 } == to_integral_pixel(greyscale_with_alpha_8{ 0b1100'1010, 0b0001'1110 }));
		}
		TEST_METHOD(to_integral_pixel_test_10) {
			Assert::IsTrue(integral_pixel_info{ 0b10'0000, 0b1100'1010'1111'1000'1010'1111'0001'1011 } == to_integral_pixel(greyscale_with_alpha_16{ 0b1100'1010'1111'1000, 0b1010'1111'0001'1011 }));
		}
		TEST_METHOD(to_integral_pixel_test_11) {
			Assert::IsTrue(integral_pixel_info{ 0b10'0000, 0b1100'1010'1111'1010'1000'0001'1110'0011 } == to_integral_pixel(truecolour_with_alpha_8{ 0b1100'1010, 0b1111'1010, 0b1000'0001, 0b1110'0011 }));
		}
		TEST_METHOD(to_integral_pixel_test_12) {
			Assert::IsTrue(integral_pixel_info{ 0b100'0000, 0b1100'1010'0011'1110'1111'1010'0000'1110'1000'0001'0011'0000'1110'1001'0110'0011 }
				== to_integral_pixel(truecolour_with_alpha_16{ 0b1100'1010'0011'1110, 0b1111'1010'0000'1110, 0b1000'0001'0011'0000, 0b1110'1001'0110'0011 }));
		}
	};
	TEST_CLASS(write_tests) {
	public:
		TEST_METHOD(write_test_1) {
			std::uint8_t buffer[4], * pos{ buffer };
			const std::uint8_t* cpos{ buffer };
			write(pos, { 32, 0xabcdef89 });
			Assert::IsTrue(read_4(cpos, { buffer, buffer + 4 }) == 0xabcdef89);
		}
		TEST_METHOD(write_test_2) {
			std::uint8_t buffer[8], * pos{ buffer };
			const std::uint8_t* cpos{ buffer };
			write(pos, { 64, 0xabcd'ef89'1234'5678 });
			Assert::IsTrue(read_8(cpos, { buffer, buffer + 8 }) == 0xabcd'ef89'1234'5678);
		}
	};
}