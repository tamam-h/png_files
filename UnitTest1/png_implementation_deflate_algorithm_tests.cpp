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
	TEST_CLASS(huffman_code_reader_tests) {
	public:
		TEST_METHOD(huffman_code_reader_test_1) {
			std::vector<code_length_t> code_lengths{ 3, 3, 3, 3, 3, 2, 4, 4 };
			std::uint8_t stream_value{ 0b1111 };
			bitwise_readable_stream stream{ { &stream_value, &stream_value + 1 } };
			huffman_code_reader reader{ { code_lengths.data(), code_lengths.data() + code_lengths.size() } };
			Assert::AreEqual(static_cast<huffman_code_t>(7), reader.peek(stream));
		}
		TEST_METHOD(huffman_code_reader_test_2) {
			std::vector<code_length_t> code_lengths{ 3, 3, 3, 3, 3, 2, 4, 4 };
			std::uint8_t stream_value{ 0b0111 };
			bitwise_readable_stream stream{ { &stream_value, &stream_value + 1 } };
			huffman_code_reader reader{ { code_lengths.data(), code_lengths.data() + code_lengths.size() } };
			Assert::AreEqual(static_cast<huffman_code_t>(6), reader.peek(stream));
		}
		TEST_METHOD(huffman_code_reader_test_3) {
			std::vector<code_length_t> code_lengths{ 3, 3, 3, 3, 3, 2, 4, 4 };
			std::uint8_t stream_value{ 0b0010 };
			bitwise_readable_stream stream{ { &stream_value, &stream_value + 1 } };
			huffman_code_reader reader{ { code_lengths.data(), code_lengths.data() + code_lengths.size() } };
			Assert::AreEqual(static_cast<huffman_code_t>(0), reader.peek(stream));
		}
		TEST_METHOD(huffman_code_reader_test_4) {
			std::vector<code_length_t> code_lengths{ 3, 3, 3, 3, 3, 2, 4, 4 };
			std::uint8_t stream_value{ 0b1010 };
			bitwise_readable_stream stream{ { &stream_value, &stream_value + 1 } };
			huffman_code_reader reader{ { code_lengths.data(), code_lengths.data() + code_lengths.size() } };
			Assert::AreEqual(static_cast<huffman_code_t>(0), reader.peek(stream));
		}
		TEST_METHOD(huffman_code_reader_test_5) {
			std::vector<code_length_t> code_lengths_1{ 3, 3, 3, 3, 3, 2, 4, 4 };
			std::vector<code_length_t> code_lengths_2{ 3, 3, 3, 3, 3, 2, 4, 4, 0, 0, 0 };
			std::uint8_t stream_value{};
			bitwise_readable_stream stream{ { &stream_value, &stream_value + 1 } };
			huffman_code_reader reader_1{ { code_lengths_1.data(), code_lengths_1.data() + code_lengths_1.size() } };
			huffman_code_reader reader_2{ { code_lengths_2.data(), code_lengths_2.data() + code_lengths_2.size() } };
			for (std::uint8_t i{ 0 }; i <= 0b1111; ++i) {
				stream_value = i;
				if (reader_1.peek(stream) != reader_2.peek(stream)) { Assert::Fail(L"should be equal for all input"); }
			}
		}
	};
	TEST_CLASS(get_length_tests) {
	public:
		TEST_METHOD(get_length_test_1) {
			std::uint8_t stream_value{ 0b1111'1011 };
			bitwise_readable_stream stream{ { &stream_value, &stream_value + 1 } };
			Assert::AreEqual(static_cast<std::uint_fast16_t>(38), get_length(stream, 273));
			Assert::AreEqual(static_cast<std::uint_fast16_t>(31), stream.peek(5), L"didn\'t read correct number of bits");
		}
	};
	TEST_CLASS(get_distance_tests) {
	public:
		TEST_METHOD(get_distance_test_1) {
			std::uint8_t stream_value{ 0b1111'1011 };
			bitwise_readable_stream stream{ { &stream_value, &stream_value + 1 } };
			Assert::AreEqual(static_cast<std::uint_fast16_t>(20), get_distance(stream, 8));
			Assert::AreEqual(static_cast<std::uint_fast16_t>(31), stream.peek(5), L"didn\'t read correct number of bits");
		}
	};
}