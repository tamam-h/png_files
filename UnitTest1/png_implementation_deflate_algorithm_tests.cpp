#include "pch.h"
#include "png_implementation_deflate_algorithm.hpp"
#include "CppUnitTest.h"
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace png_implementation_deflate_algorithm_tests {
	TEST_CLASS(bit_reverse_tests) {
	public:
		TEST_METHOD(bit_reverse_test_1) {
			Assert::AreEqual(static_cast<huffman_code_t>(0b0), bit_reverse(0b0, 0));
		}
		TEST_METHOD(bit_reverse_test_2) {
			Assert::AreEqual(static_cast<huffman_code_t>(0b0), bit_reverse(0b0, 1));
		}
		TEST_METHOD(bit_reverse_test_3) {
			Assert::AreEqual(static_cast<huffman_code_t>(0b1), bit_reverse(0b1, 1));
		}
		TEST_METHOD(bit_reverse_test_4) {
			Assert::AreEqual(static_cast<huffman_code_t>(0b1'0011), bit_reverse(0b1'1001, 5));
		}
		TEST_METHOD(bit_reverse_test_5) {
			Assert::AreEqual(static_cast<huffman_code_t>(0b10'0100'1100), bit_reverse(0b00'1100'1001, 10));
		}
		TEST_METHOD(bit_reverse_test_6) {
			Assert::AreEqual(static_cast<huffman_code_t>(0b100'1001'1101'1011), bit_reverse(0b110'1101'1100'1001, 15));
		}
	};
}