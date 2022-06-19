#include "pch.h"
#include "png_implementation_bitwise_readable_stream.hpp"
#include "png_implementation_deflate_algorithm.hpp"
#include "CppUnitTest.h"
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace png_implementation_deflate_algorithm_tests {
	bool operator==(const std::span<const std::uint8_t>& first, const std::span<const std::uint8_t>& second) {
		if (first.size() != second.size()) { return 0; }
		for (int i{ 0 }; i < first.size(); ++i) {
			if (first[i] != second[i]) { return 0; }
		}
		return 1;
	}
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
	TEST_CLASS(huffman_code_table_tests) {
	public:
		TEST_METHOD(huffman_code_table_test_1) {
			std::vector<code_length_t> code_lengths{ 3, 3, 3, 3, 3, 2, 4, 4 };
			std::uint8_t stream_value{ 0b1111 };
			bitwise_readable_stream stream{ { &stream_value, &stream_value + 1 } };
			huffman_code_table reader{ { code_lengths.data(), code_lengths.data() + code_lengths.size() } };
			Assert::AreEqual(static_cast<huffman_code_t>(7), reader.peek(stream));
		}
		TEST_METHOD(huffman_code_table_test_2) {
			std::vector<code_length_t> code_lengths{ 3, 3, 3, 3, 3, 2, 4, 4 };
			std::uint8_t stream_value{ 0b0111 };
			bitwise_readable_stream stream{ { &stream_value, &stream_value + 1 } };
			huffman_code_table reader{ { code_lengths.data(), code_lengths.data() + code_lengths.size() } };
			Assert::AreEqual(static_cast<huffman_code_t>(6), reader.peek(stream));
		}
		TEST_METHOD(huffman_code_table_test_3) {
			std::vector<code_length_t> code_lengths{ 3, 3, 3, 3, 3, 2, 4, 4 };
			std::uint8_t stream_value{ 0b0010 };
			bitwise_readable_stream stream{ { &stream_value, &stream_value + 1 } };
			huffman_code_table reader{ { code_lengths.data(), code_lengths.data() + code_lengths.size() } };
			Assert::AreEqual(static_cast<huffman_code_t>(0), reader.peek(stream));
		}
		TEST_METHOD(huffman_code_table_test_4) {
			std::vector<code_length_t> code_lengths{ 3, 3, 3, 3, 3, 2, 4, 4 };
			std::uint8_t stream_value{ 0b1010 };
			bitwise_readable_stream stream{ { &stream_value, &stream_value + 1 } };
			huffman_code_table reader{ { code_lengths.data(), code_lengths.data() + code_lengths.size() } };
			Assert::AreEqual(static_cast<huffman_code_t>(0), reader.peek(stream));
		}
		TEST_METHOD(huffman_code_table_test_5) {
			std::vector<code_length_t> code_lengths_1{ 3, 3, 3, 3, 3, 2, 4, 4 };
			std::vector<code_length_t> code_lengths_2{ 3, 3, 3, 3, 3, 2, 4, 4, 0, 0, 0 };
			std::uint8_t stream_value{};
			bitwise_readable_stream stream{ { &stream_value, &stream_value + 1 } };
			huffman_code_table reader_1{ { code_lengths_1.data(), code_lengths_1.data() + code_lengths_1.size() } };
			huffman_code_table reader_2{ { code_lengths_2.data(), code_lengths_2.data() + code_lengths_2.size() } };
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
			Assert::IsTrue(stream.can_advance(5), L"didn\'t read correct number of bits");
			Assert::AreEqual(static_cast<std::uint_fast16_t>(31), stream.peek(5), L"didn\'t read correct number of bits");
		}
		TEST_METHOD(get_length_test_2) {
			std::uint8_t stream_value{ 0b1111'1011 };
			bitwise_readable_stream stream{ { &stream_value, &stream_value + 1 } };
			bool should_fail{ 1 };
			try {
				get_length(stream, 286);
			} catch (...) {
				should_fail = 0;
			}
			Assert::IsFalse(should_fail);
		}
	};
	TEST_CLASS(get_distance_tests) {
	public:
		TEST_METHOD(get_distance_test_1) {
			std::uint8_t stream_value{ 0b1111'1011 };
			bitwise_readable_stream stream{ { &stream_value, &stream_value + 1 } };
			Assert::AreEqual(static_cast<std::uint_fast16_t>(20), get_distance(stream, 8));
			Assert::IsTrue(stream.can_advance(5), L"didn\'t read correct number of bits");
			Assert::AreEqual(static_cast<std::uint_fast16_t>(31), stream.peek(5), L"didn\'t read correct number of bits");
		}
		TEST_METHOD(get_length_test_2) {
			std::uint8_t stream_value{ 0b1111'1011 };
			bitwise_readable_stream stream{ { &stream_value, &stream_value + 1 } };
			bool should_fail{ 1 };
			try {
				get_distance(stream, 30);
			} catch (...) {
				should_fail = 0;
			}
			Assert::IsFalse(should_fail);
		}
	};
	TEST_CLASS(handle_uncompressed_block_tests) {
	public:
		TEST_METHOD(handle_uncompressed_block_test_1) {
			std::uint8_t arr[]{ 0b0000'0000, 0x05, 0x00, 0xFA, 0xFF, 1, 2, 3, 4, 5, 6 };
			std::vector<std::uint8_t> out{ 27 };
			bitwise_readable_stream compressed{ { arr, arr + sizeof(arr) } };
			compressed.advance(1);
			handle_uncompressed_block(out, compressed);
			if (out != std::vector<std::uint8_t>{ 27, 1, 2, 3, 4, 5 }) {
				Assert::Fail(L"output is not equal");
			}
			Assert::IsTrue(compressed.can_advance(8));
			Assert::AreEqual(static_cast<std::uint_fast16_t>(6), compressed.peek(8));
		}
		TEST_METHOD(handle_uncompressed_block_test_2) {
			std::uint8_t arr[]{ 0b0000'0000, 0x05, 0x00, 0xFA, 0xFF, 1, 2, 3, 4 };
			std::vector<std::uint8_t> out{ 27 };
			bitwise_readable_stream compressed{ { arr, arr + sizeof(arr) } };
			compressed.advance(1);
			bool should_fail{ 1 };
			try {
				handle_uncompressed_block(out, compressed);
			} catch (...) {
				should_fail = 0;
			}
			Assert::IsFalse(should_fail);
		}
		TEST_METHOD(handle_uncompressed_block_test_3) {
			std::uint8_t arr[]{ 0b0000'0000, 0x00, 0x05, 0xFF, 0xFF, 1, 2, 3, 4, 5 };
			std::vector<std::uint8_t> out{ 27 };
			bitwise_readable_stream compressed{ { arr, arr + sizeof(arr) } };
			compressed.advance(1);
			bool should_fail{ 1 };
			try {
				handle_uncompressed_block(out, compressed);
			} catch (...) {
				should_fail = 0;
			}
			Assert::IsFalse(should_fail);
		}
	};
	TEST_CLASS(handle_code_length_code_tests) {
	public:
		TEST_METHOD(handle_code_length_code_test_1) {
			std::vector<code_length_t> out{ 5 };
			std::uint8_t stream_contents{ 13 };
			bitwise_readable_stream stream{ { &stream_contents, &stream_contents + 1 } };
			handle_code_length_code(out, 15, stream);
			Assert::IsTrue(out == std::vector<code_length_t>{ 5, 15 });
			Assert::IsTrue(stream.can_advance(8));
			Assert::AreEqual(static_cast<std::uint_fast16_t>(13), stream.peek(8));
		}
		TEST_METHOD(handle_code_length_code_test_2) {
			std::vector<code_length_t> out{ 5 };
			std::uint8_t stream_contents{ 0b1111'1101 };
			bitwise_readable_stream stream{ { &stream_contents, &stream_contents + 1 } };
			handle_code_length_code(out, 16, stream);
			Assert::IsTrue(out == std::vector<code_length_t>{ 5, 5, 5, 5, 5 });
			Assert::IsTrue(stream.can_advance(6));
			Assert::AreEqual(static_cast<std::uint_fast16_t>(0b11'1111), stream.peek(6));
		}
		TEST_METHOD(handle_code_length_code_test_3) {
			std::vector<code_length_t> out{};
			std::uint8_t stream_contents{ 0b1111'1101 };
			bitwise_readable_stream stream{ { &stream_contents, &stream_contents + 1 } };
			bool should_fail{ 1 };
			try {
				handle_code_length_code(out, 16, stream);
			} catch (...) {
				should_fail = 0;
			}
			Assert::IsFalse(should_fail);
		}
		TEST_METHOD(handle_code_length_code_test_4) {
			std::vector<code_length_t> out{ 5 };
			std::uint8_t stream_contents{ 0b1111'1001 };
			bitwise_readable_stream stream{ { &stream_contents, &stream_contents + 1 } };
			handle_code_length_code(out, 17, stream);
			Assert::IsTrue(out == std::vector<code_length_t>{ 5, 0, 0, 0, 0 });
			Assert::IsTrue(stream.can_advance(5));
			Assert::AreEqual(static_cast<std::uint_fast16_t>(0b1'1111), stream.peek(5));
		}
	};
	TEST_CLASS(huffman_code_reader_tests) {
	public:
		TEST_METHOD(huffman_code_reader_test_1) {
			std::vector<code_length_t> code_lengths{ 3, 3, 3, 3, 3, 2, 4, 4 };
			huffman_code_reader reader; reader.code_lengths = code_lengths;
			reader.set_table();
			std::uint8_t stream_value{ 0b1111'0111 };
			bitwise_readable_stream stream{ { &stream_value, &stream_value + 1 } };
			Assert::AreEqual(static_cast<huffman_code_t>(6), reader.read(stream));
			Assert::IsTrue(stream.can_advance(4));
			Assert::AreEqual(static_cast<std::uint_fast16_t>(0b1111), stream.peek(4));
		}
	};
	TEST_CLASS(compress_tests) {
	public:
		TEST_METHOD(compress_test_1) {
				std::uint8_t buffer[500], buffer2[505]{ 1, 2, 3, 4, 5 };
				for (auto& i : buffer) { i = std::rand() % UINT8_MAX; }
				std::memcpy(buffer2 + 5, buffer, 500);
				std::vector<std::uint8_t> compressed, decompressed{ 1, 2, 3, 4, 5 };
				compress(compressed, { buffer, buffer + 500 });
				bitwise_readable_stream compressed_stream{ { compressed.data(), compressed.data() + compressed.size() } };
				try {
					decompress(decompressed, compressed_stream);
				} catch (...) {
					Assert::Fail(L"should not throw");
				}
				Assert::IsTrue(std::span<const std::uint8_t>{ buffer2, buffer2 + 505 } == std::span<const std::uint8_t>{ decompressed.data(), decompressed.data() + decompressed.size() });
			}
			TEST_METHOD(compress_test_2) {
				std::uint8_t buffer[500], buffer2[1005]{ 1, 2, 3, 4, 5 };
				for (auto& i : buffer) { i = std::rand() % UINT8_MAX; }
				std::memcpy(buffer2 + 5, buffer, 500);
				std::memcpy(buffer2 + 505, buffer, 500);
				std::vector<std::uint8_t> compressed, decompressed{ 1, 2, 3, 4, 5 };
				compress(compressed, { buffer, buffer + 500 });
				compress(compressed, { buffer, buffer + 500 });
				bitwise_readable_stream compressed_stream{ { compressed.data(), compressed.data() + compressed.size() } };
				try {
					decompress(decompressed, compressed_stream);
					decompress(decompressed, compressed_stream);
				} catch (...) {
					Assert::Fail(L"should not throw");
				}
				Assert::IsTrue(std::span<const std::uint8_t>{ buffer2, buffer2 + 1005 } == std::span<const std::uint8_t>{ decompressed.data(), decompressed.data() + decompressed.size() });
			}
			TEST_METHOD(compress_test_3) {
				std::vector<std::uint8_t> buffer, buffer2; buffer.resize(100'000);
				for (auto& i : buffer) { i = std::rand() % UINT8_MAX; }
				buffer2 = buffer;
				std::vector<std::uint8_t> compressed, decompressed;
				compress(compressed, { buffer.data(), buffer.data() + 100'000 });
				bitwise_readable_stream compressed_stream{ { compressed.data(), compressed.data() + compressed.size() } };
				try {
					decompress(decompressed, compressed_stream);
				} catch (...) {
					Assert::Fail(L"should not throw");
				}
				Assert::IsTrue(buffer2 == decompressed, L"should be equal");
			}
	};
}