#include "pch.h"
#include "png_implementation_bitwise_readable_stream.hpp"
#include "CppUnitTest.h"
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

bool operator==(const std::span<const std::uint8_t>& first, const std::span<const std::uint8_t>& second) {
	if (first.size() != second.size()) { return 0; }
	for (int i{ 0 }; i < first.size(); ++i) {
		if (first[i] != second[i]) { return 0; }
	}
	return 1;
}

namespace png_implementation_bitwise_readable_stream_tests {
	TEST_CLASS(bitwise_readable_stream_tests) {
	public:
		TEST_METHOD(peek_test_1) {
			std::uint8_t buffer[3]{ 0b0111'0111, 0b1101'0000, 0b0010'1110 };
			bitwise_readable_stream curr{ { buffer, buffer + 3 } };
			Assert::AreEqual(static_cast<std::uint_fast16_t>(1), curr.peek(1), L"should be equal");
		}
		TEST_METHOD(peek_test_2) {
			std::uint8_t buffer[3]{ 0b0111'0111, 0b1101'0010, 0b0010'1110 };
			bitwise_readable_stream curr{ { buffer, buffer + 3 } };
			Assert::AreEqual(static_cast<std::uint_fast16_t>(0b0010'0111'0111), curr.peek(12), L"should be equal");
		}
		TEST_METHOD(peek_test_3) {
			std::uint8_t buffer[3]{ 0b0111'0111, 0b1101'0000, 0b0010'1110 };
			bitwise_readable_stream curr{ { buffer, buffer + 3 } };
			curr.advance(6);
			Assert::AreEqual(static_cast<std::uint_fast16_t>(0b1011'0100'0001), curr.peek(12), L"should be equal");
		}
		TEST_METHOD(peek_test_4) {
			std::uint8_t buffer[3]{ 0b0111'0111, 0b1101'0000, 0b0010'1110 };
			bitwise_readable_stream curr{ { buffer, buffer + 3 } };
			curr.advance(12);
			Assert::AreEqual(static_cast<std::uint_fast16_t>(0b0010'1110'1101), curr.peek(12), L"should be equal");
		}
		TEST_METHOD(peek_test_5) {
			std::uint8_t buffer[3]{ 0b0111'0111, 0b1101'0000, 0b0010'1110 };
			bitwise_readable_stream curr{ { buffer, buffer + 3 } };
			curr.advance(3);
			Assert::AreEqual(static_cast<std::uint_fast16_t>(0b1110), curr.peek(4), L"should be equal");
		}
		TEST_METHOD(peek_test_6) {
			std::uint8_t buffer[3]{ 0b0111'0111, 0b1101'0000, 0b0010'1110 };
			bitwise_readable_stream curr{ { buffer, buffer + 3 } };
			curr.advance(20);
			Assert::AreEqual(static_cast<std::uint_fast16_t>(0b0010), curr.peek(4), L"should be equal");
		}
		TEST_METHOD(peek_test_7) {
			std::uint8_t buffer[3]{ 0b0111'0111, 0b1101'0001, 0b0010'1110 };
			bitwise_readable_stream curr{ { buffer, buffer + 3 } };
			curr.advance(6);
			Assert::AreEqual(static_cast<std::uint_fast16_t>(0b01'01), curr.peek(4), L"should be equal");
		}
		TEST_METHOD(peek_test_8) {
			std::uint8_t buffer[3]{ 0b0111'0111, 0b1101'0001, 0b0010'1110 };
			bitwise_readable_stream curr{ { buffer, buffer + 3 } };
			Assert::AreEqual(static_cast<std::uint_fast16_t>(0b1101'0001'0111'0111), curr.peek(16), L"should be equal");
		}
		TEST_METHOD(peek_test_9) {
			std::uint8_t buffer[3]{ 0b0111'0111, 0b1101'0001, 0b0010'1110 };
			bitwise_readable_stream curr{ { buffer, buffer + 3 } };
			curr.advance(1);
			Assert::AreEqual(static_cast<std::uint_fast16_t>(0b0110'1000'1011'1011), curr.peek(16), L"should be equal");
		}
		TEST_METHOD(peek_test_10) {
			std::uint8_t buffer[3]{ 0b0111'0111, 0b1101'0001, 0b0010'1110 };
			bitwise_readable_stream curr{ { buffer, buffer + 3 } };
			curr.advance(8);
			Assert::AreEqual(static_cast<std::uint_fast16_t>(0b0010'1110'1101'0001), curr.peek(16), L"should be equal");
		}
		TEST_METHOD(can_advance_test_1) {
			std::uint8_t buffer[3]{ 0b0111'0111, 0b1101'0001, 0b0010'1110 };
			bitwise_readable_stream curr{ { buffer, buffer + 3 } };
			curr.advance(8);
			Assert::IsTrue(curr.can_advance(16));
		}
		TEST_METHOD(can_advance_test_2) {
			std::uint8_t buffer[3]{ 0b0111'0111, 0b1101'0001, 0b0010'1110 };
			bitwise_readable_stream curr{ { buffer, buffer + 3 } };
			curr.advance(8);
			Assert::IsFalse(curr.can_advance(17));
		}
		TEST_METHOD(can_advance_test_3) {
			std::uint8_t buffer[3]{ 0b0111'0111, 0b1101'0001, 0b0010'1110 };
			bitwise_readable_stream curr{ { buffer, buffer + 3 } };
			curr.advance(1);
			Assert::IsFalse(curr.can_advance(24));
		}
		TEST_METHOD(can_advance_test_4) {
			std::uint8_t buffer[3]{ 0b0111'0111, 0b1101'0001, 0b0010'1110 };
			bitwise_readable_stream curr{ { buffer, buffer + 3 } };
			curr.advance(0);
			Assert::IsTrue(curr.can_advance(24));
		}
		TEST_METHOD(zero_extended_peek_test_1) {
			std::uint8_t buffer[3]{ 0b0111'0111, 0b1101'0000, 0b0010'1110 };
			bitwise_readable_stream curr{ { buffer, buffer + 3 } };
			Assert::AreEqual(static_cast<std::uint_fast16_t>(1), curr.zero_extended_peek(1), L"should be equal");
		}
		TEST_METHOD(zero_extended_peek_test_2) {
			std::uint8_t buffer[3]{ 0b0111'0111, 0b1101'0010, 0b0010'1110 };
			bitwise_readable_stream curr{ { buffer, buffer + 3 } };
			Assert::AreEqual(static_cast<std::uint_fast16_t>(0b0010'0111'0111), curr.zero_extended_peek(12), L"should be equal");
		}
		TEST_METHOD(zero_extended_peek_test_3) {
			std::uint8_t buffer[3]{ 0b0111'0111, 0b1101'0000, 0b0010'1110 };
			bitwise_readable_stream curr{ { buffer, buffer + 3 } };
			curr.advance(6);
			Assert::AreEqual(static_cast<std::uint_fast16_t>(0b1011'0100'0001), curr.zero_extended_peek(12), L"should be equal");
		}
		TEST_METHOD(zero_extended_peek_test_4) {
			std::uint8_t buffer[3]{ 0b0111'0111, 0b1101'0000, 0b0010'1110 };
			bitwise_readable_stream curr{ { buffer, buffer + 3 } };
			curr.advance(12);
			Assert::AreEqual(static_cast<std::uint_fast16_t>(0b0010'1110'1101), curr.zero_extended_peek(12), L"should be equal");
		}
		TEST_METHOD(zero_extended_peek_test_5) {
			std::uint8_t buffer[3]{ 0b0111'0111, 0b1101'0000, 0b0010'1110 };
			bitwise_readable_stream curr{ { buffer, buffer + 3 } };
			curr.advance(3);
			Assert::AreEqual(static_cast<std::uint_fast16_t>(0b1110), curr.zero_extended_peek(4), L"should be equal");
		}
		TEST_METHOD(zero_extended_peek_test_6) {
			std::uint8_t buffer[3]{ 0b0111'0111, 0b1101'0000, 0b0010'1110 };
			bitwise_readable_stream curr{ { buffer, buffer + 3 } };
			curr.advance(20);
			Assert::AreEqual(static_cast<std::uint_fast16_t>(0b0010), curr.zero_extended_peek(4), L"should be equal");
		}
		TEST_METHOD(zero_extended_peek_test_7) {
			std::uint8_t buffer[3]{ 0b0111'0111, 0b1101'0001, 0b0010'1110 };
			bitwise_readable_stream curr{ { buffer, buffer + 3 } };
			curr.advance(6);
			Assert::AreEqual(static_cast<std::uint_fast16_t>(0b01'01), curr.zero_extended_peek(4), L"should be equal");
		}
		TEST_METHOD(zero_extended_peek_test_8) {
			std::uint8_t buffer[3]{ 0b0111'0111, 0b1101'0001, 0b0010'1110 };
			bitwise_readable_stream curr{ { buffer, buffer + 3 } };
			Assert::AreEqual(static_cast<std::uint_fast16_t>(0b1101'0001'0111'0111), curr.zero_extended_peek(16), L"should be equal");
		}
		TEST_METHOD(zero_extended_peek_test_9) {
			std::uint8_t buffer[3]{ 0b0111'0111, 0b1101'0001, 0b0010'1110 };
			bitwise_readable_stream curr{ { buffer, buffer + 3 } };
			curr.advance(1);
			Assert::AreEqual(static_cast<std::uint_fast16_t>(0b0110'1000'1011'1011), curr.zero_extended_peek(16), L"should be equal");
		}
		TEST_METHOD(zero_extended_peek_test_10) {
			std::uint8_t buffer[3]{ 0b0111'0111, 0b1101'0001, 0b0010'1110 };
			bitwise_readable_stream curr{ { buffer, buffer + 3 } };
			curr.advance(8);
			Assert::AreEqual(static_cast<std::uint_fast16_t>(0b0010'1110'1101'0001), curr.zero_extended_peek(16), L"should be equal");
		}
		TEST_METHOD(zero_extended_peek_test_11) {
			std::uint8_t buffer[3]{ 0b0111'0111, 0b1101'0001, 0b0010'1110 };
			bitwise_readable_stream curr{ { buffer, buffer + 3 } };
			curr.advance(24);
			Assert::AreEqual(static_cast<std::uint_fast16_t>(0), curr.zero_extended_peek(16), L"should be equal");
		}
		TEST_METHOD(zero_extended_peek_test_12) {
			std::uint8_t buffer[3]{ 0b0111'0111, 0b1101'0001, 0b0010'1110 };
			bitwise_readable_stream curr{ { buffer, buffer + 3 } };
			curr.advance(24);
			Assert::AreEqual(static_cast<std::uint_fast16_t>(0), curr.zero_extended_peek(4), L"should be equal");
		}
		TEST_METHOD(zero_extended_peek_test_13) {
			std::uint8_t buffer[3]{ 0b0111'0111, 0b1101'0001, 0b0010'1110 };
			bitwise_readable_stream curr{ { buffer, buffer + 3 } };
			curr.advance(9);
			Assert::AreEqual(static_cast<std::uint_fast16_t>(0b0001'0111'0110'1000), curr.zero_extended_peek(16), L"should be equal");
		}
		TEST_METHOD(advance_to_byte_test_1) {
			std::uint8_t buffer[2]{ 0b1001'1110, 0b1110'0001 };
			bitwise_readable_stream curr{ { buffer, buffer + 2 } };
			curr.advance_to_byte();
			Assert::AreEqual(static_cast<std::uint_fast16_t>(0b1001'1110), curr.peek(8));
		}
		TEST_METHOD(advance_to_byte_test_2) {
			std::uint8_t buffer[2]{ 0b1001'1110, 0b1110'0001 };
			bitwise_readable_stream curr{ { buffer, buffer + 2 } };
			curr.advance(1);
			curr.advance_to_byte();
			Assert::AreEqual(static_cast<std::uint_fast16_t>(0b1110'0001), curr.peek(8));
		}
		TEST_METHOD(advance_to_byte_test_3) {
			std::uint8_t buffer[2]{ 0b1001'1110, 0b1110'0001 };
			bitwise_readable_stream curr{ { buffer, buffer + 2 } };
			curr.advance(16);
			curr.advance_to_byte();
			Assert::IsFalse(curr.can_advance(1));
		}
		TEST_METHOD(get_position_test_1) {
			std::uint8_t buffer[25]{};
			bitwise_readable_stream curr{ { buffer, buffer + 25 } };
			curr.advance(23 * 8);
			Assert::AreEqual(static_cast<const std::uint8_t*>(buffer + 23), curr.get_position());
		}
	};
}
