#include "pch.h"
#include "png_implementation_pixel_cast.hpp"
#include <CppUnitTest.h>
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace pixel_cast_tests {
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
}