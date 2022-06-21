#include "pch.h"
#include "png_implementation_image_data.hpp"
#include "png_implementation_pixel_cast.hpp"
#include "png_implementation_pixel_types.hpp"
#include <CppUnitTest.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace image_data_tests {
	TEST_CLASS(get_pixel_type_tests) {
	public:
		TEST_METHOD(get_pixel_type_test_1) {
			image_data curr;
			curr.convert_to<greyscale_with_alpha_float>();
			Assert::IsTrue(pixel_type_number::greyscale_with_alpha_float == curr.get_pixel_type());
		}
		TEST_METHOD(get_pixel_type_test_2) {
			image_data curr;
			curr.convert_to<truecolour_with_alpha_8>();
			Assert::IsTrue(pixel_type_number::truecolour_with_alpha_8 == curr.get_pixel_type());
		}
	};
	TEST_CLASS(convert_to_tests) {
	public:
		TEST_METHOD(convert_to_test_1) {
			std::vector<std::vector<greyscale_16>> grid(2000, std::vector<greyscale_16>(2000));
			image_data curr; curr.get_array<greyscale_16>() = grid;
			std::clock_t start{ std::clock() };
			curr.convert_to<greyscale_16>();
			curr.get_array<greyscale_16>();
			double time{ static_cast<double>(std::clock() - start) / CLOCKS_PER_SEC };
			Assert::IsTrue(time <= 0.001);
		}
		TEST_METHOD(convert_to_test_2) {
			std::vector<std::vector<greyscale_16>> grid(25, std::vector<greyscale_16>(25));
			for (auto&& i : grid) {
				for (auto&& j : i) { j.grey = std::rand() % UINT16_MAX; }
			}
			image_data curr; curr.get_array<greyscale_16>() = grid;
			curr.convert_to<greyscale_8>();
			std::vector<std::vector<greyscale_8>>& arr{ curr.get_array<greyscale_8>() };
			for (long i{ 0 }; i < 25; ++i) {
				for (long j{ 0 }; j < 25; ++j) {
					Assert::IsTrue(arr[i][j].grey == pixel_cast<greyscale_8>(grid[i][j]).grey);
				}
			}
		}
	};
	TEST_CLASS(get_array_tests) {
	public:
		TEST_METHOD(get_array_test_1) {
			std::vector<std::vector<truecolour_8>> grid(25, std::vector<truecolour_8>(25));
			for (auto&& i : grid) {
				for (auto&& j : i) {
					j.red = std::rand() % UINT8_MAX;
					j.green = std::rand() % UINT8_MAX;
					j.blue = std::rand() % UINT8_MAX;
				}
			}
			image_data curr; curr.get_array<truecolour_8>() = grid;
			curr.convert_to<truecolour_with_alpha_float>();
			std::vector<std::vector<truecolour_8>>& arr{ curr.get_array<truecolour_8>() };
			for (long i{ 0 }; i < 25; ++i) {
				for (long j{ 0 }; j < 25; ++j) {
					Assert::IsTrue(arr[i][j].red == grid[i][j].red);
					Assert::IsTrue(arr[i][j].green == grid[i][j].green);
					Assert::IsTrue(arr[i][j].blue == grid[i][j].blue);
				}
			}
		}
		TEST_METHOD(get_array_test_2) {
			image_data curr; curr.convert_to<truecolour_8>();
			const image_data& ref{ curr };
			ref.get_array<truecolour_8>();
		}
		TEST_METHOD(get_array_test_3) {
			image_data curr; curr.convert_to<truecolour_with_alpha_float>();
			bool should_fail{ 1 };
			const image_data& ref{ curr };
			try {
				ref.get_array<truecolour_8>();
			} catch (...) {
				should_fail = 0;
			}
			Assert::IsFalse(should_fail);
		}
	};
}