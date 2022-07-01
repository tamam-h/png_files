#include "pch.h"
#include "png.hpp"
#include <CppUnitTest.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace png_tests {
	namespace png_tests {
		TEST_CLASS(get_pixel_type_tests) {
		public:
			TEST_METHOD(get_pixel_type_test_1) {
				png curr;
				curr.convert_to<greyscale_with_alpha_16>();
				Assert::IsTrue(curr.get_pixel_type() == pixel_type_number::greyscale_with_alpha_16);
			}
			TEST_METHOD(get_pixel_type_test_2) {
				png curr;
				curr.convert_to<truecolour_with_alpha_16>();
				Assert::IsTrue(curr.get_pixel_type() == pixel_type_number::truecolour_with_alpha_16);
			}
		};
		TEST_CLASS(convert_to_tests) {
		public:
			TEST_METHOD(convert_to_test_1) {
				png curr;
				curr.convert_to<greyscale_with_alpha_8>();
				Assert::IsTrue(curr.get_pixel_type() == pixel_type_number::greyscale_with_alpha_8);
			}
			TEST_METHOD(convert_to_test_2) {
				png curr;
				curr.convert_to(pixel_type_number::truecolour_with_alpha_16);
				Assert::IsTrue(curr.get_pixel_type() == pixel_type_number::truecolour_with_alpha_16);
			}
			TEST_METHOD(convert_to_test_3) {
				png curr;
				curr.convert_to(pixel_type_number::truecolour_8);
				Assert::IsTrue(curr.get_pixel_type() == pixel_type_number::truecolour_8);
			}
		};
		TEST_CLASS(write_to_tests) {
		public:
			TEST_METHOD(write_to_test_1) {
				png curr;
				std::vector<std::vector<greyscale_1>>& arr{ curr.get_array<greyscale_1>() };
				arr.resize(10, std::vector<greyscale_1>(12));
				for (std::vector<greyscale_1>& i : arr) {
					for (greyscale_1& j : i) {
						j.grey = std::rand() & 0b1;
					}
				}
				std::vector<std::uint8_t> buffer{ '7' };
				curr.write_to(buffer);
				png other{ { buffer.data() + 1, buffer.data() + buffer.size() } };
				std::vector<std::vector<greyscale_1>>& other_arr{ other.get_array<greyscale_1>() };
				for (std::uint_fast32_t i{ 0 }; i < 10; ++i) {
					for (std::uint_fast32_t j{ 0 }; j < 12; ++j) {
						Assert::IsTrue(other_arr[i][j].grey == arr[i][j].grey);
					}
				}
			}
			TEST_METHOD(write_to_test_2) {
				png curr;
				std::vector<std::vector<greyscale_2>>& arr{ curr.get_array<greyscale_2>() };
				arr.resize(10, std::vector<greyscale_2>(12));
				for (std::vector<greyscale_2>& i : arr) {
					for (greyscale_2& j : i) {
						j.grey = std::rand() & 0b11;
					}
				}
				std::vector<std::uint8_t> buffer{ '7' };
				curr.write_to(buffer);
				png other{ { buffer.data() + 1, buffer.data() + buffer.size() } };
				std::vector<std::vector<greyscale_2>>& other_arr{ other.get_array<greyscale_2>() };
				for (std::uint_fast32_t i{ 0 }; i < 10; ++i) {
					for (std::uint_fast32_t j{ 0 }; j < 12; ++j) {
						Assert::IsTrue(other_arr[i][j].grey == arr[i][j].grey);
					}
				}
			}
			TEST_METHOD(write_to_test_3) {
				png curr;
				std::vector<std::vector<greyscale_8>>& arr{ curr.get_array<greyscale_8>() };
				arr.resize(10, std::vector<greyscale_8>(12));
				for (std::vector<greyscale_8>& i : arr) {
					for (greyscale_8& j : i) {
						j.grey = std::rand() & 0xff;
					}
				}
				std::vector<std::uint8_t> buffer{ '7' };
				curr.write_to(buffer);
				png other{ { buffer.data() + 1, buffer.data() + buffer.size() } };
				std::vector<std::vector<greyscale_8>>& other_arr{ other.get_array<greyscale_8>() };
				for (std::uint_fast32_t i{ 0 }; i < 10; ++i) {
					for (std::uint_fast32_t j{ 0 }; j < 12; ++j) {
						Assert::IsTrue(other_arr[i][j].grey == arr[i][j].grey);
					}
				}
			}
			TEST_METHOD(write_to_test_4) {
				png curr;
				std::vector<std::vector<truecolour_16>>& arr{ curr.get_array<truecolour_16>() };
				arr.resize(10, std::vector<truecolour_16>(12));
				for (std::vector<truecolour_16>& i : arr) {
					for (truecolour_16& j : i) {
						j.red = std::rand() & 0xffff;
						j.green = std::rand() & 0xffff;
						j.blue = std::rand() & 0xffff;
					}
				}
				std::vector<std::uint8_t> buffer{ '7' };
				curr.write_to(buffer);
				png other{ { buffer.data() + 1, buffer.data() + buffer.size() } };
				std::vector<std::vector<truecolour_16>>& other_arr{ other.get_array<truecolour_16>() };
				for (std::uint_fast32_t i{ 0 }; i < 10; ++i) {
					for (std::uint_fast32_t j{ 0 }; j < 12; ++j) {
						Assert::IsTrue(other_arr[i][j].red == arr[i][j].red);
						Assert::IsTrue(other_arr[i][j].green == arr[i][j].green);
						Assert::IsTrue(other_arr[i][j].blue == arr[i][j].blue);
					}
				}
			}
			TEST_METHOD(write_to_test_5) {
				png curr;
				std::vector<std::vector<truecolour_with_alpha_8>>& arr{ curr.get_array<truecolour_with_alpha_8>() };
				arr.resize(10, std::vector<truecolour_with_alpha_8>(12));
				for (std::vector<truecolour_with_alpha_8>& i : arr) {
					for (truecolour_with_alpha_8& j : i) {
						j.red = std::rand() & 0xff;
						j.green = std::rand() & 0xff;
						j.blue = std::rand() & 0xff;
						j.alpha = std::rand() & 0xff;
					}
				}
				std::vector<std::uint8_t> buffer{ '7' };
				curr.write_to(buffer);
				png other{ { buffer.data() + 1, buffer.data() + buffer.size() } };
				std::vector<std::vector<truecolour_with_alpha_8>>& other_arr{ other.get_array<truecolour_with_alpha_8>() };
				for (std::uint_fast32_t i{ 0 }; i < 10; ++i) {
					for (std::uint_fast32_t j{ 0 }; j < 12; ++j) {
						Assert::IsTrue(other_arr[i][j].red == arr[i][j].red);
						Assert::IsTrue(other_arr[i][j].green == arr[i][j].green);
						Assert::IsTrue(other_arr[i][j].blue == arr[i][j].blue);
						Assert::IsTrue(other_arr[i][j].alpha == arr[i][j].alpha);
					}
				}
			}
		};
	}
}