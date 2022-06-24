#include "pch.h"
#include "png_implementation_chunk_factory.hpp"
#include "png_implementation_chunk_types.hpp"
#include <CppUnitTest.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace chunk_types_tests {
	TEST_CLASS(construct_IHDR_tests) {
	public:
		TEST_METHOD(construct_IHDR_test_1) {
			std::vector<std::uint8_t> data(13);
			std::span<std::uint8_t> span{ data.data(), data.data() + data.size() };
			std::uint8_t* position{ data.data() };
			write_4(234, position, span);
			write_4(424, position, span);
			write_1(16, position, span);
			write_1(4, position, span);
			std::vector<std::unique_ptr<chunk_base>> vector;
			construct_IHDR(span, vector);
		}
		TEST_METHOD(construct_IHDR_test_2) {
			std::vector<std::uint8_t> data(13);
			std::span<std::uint8_t> span{ data.data(), data.data() + data.size() };
			std::uint8_t* position{ data.data() };
			bool should_fail{ 1 };
			write_4(0, position, span);
			write_4(424, position, span);
			write_1(16, position, span);
			write_1(4, position, span);
			std::vector<std::unique_ptr<chunk_base>> vector;
			try {
				construct_IHDR(span, vector);
			} catch (...) {
				should_fail = 0;
			}
			Assert::IsFalse(should_fail);
		}
		TEST_METHOD(construct_IHDR_test_3) {
			std::vector<std::uint8_t> data(13);
			std::span<std::uint8_t> span{ data.data(), data.data() + data.size() };
			std::uint8_t* position{ data.data() };
			bool should_fail{ 1 };
			write_4(18, position, span);
			write_4(424, position, span);
			write_1(16, position, span);
			write_1(3, position, span);
			std::vector<std::unique_ptr<chunk_base>> vector;
			try {
				construct_IHDR(span, vector);
			} catch (...) {
				should_fail = 0;
			}
			Assert::IsFalse(should_fail);
		}
		TEST_METHOD(construct_IHDR_test_4) {
			std::vector<std::uint8_t> data(13);
			std::span<std::uint8_t> span{ data.data(), data.data() + data.size() };
			std::uint8_t* position{ data.data() };
			bool should_fail{ 1 };
			write_4(18, position, span);
			write_4(424, position, span);
			write_1(16, position, span);
			write_1(4, position, span);
			write_1(4, position, span);
			std::vector<std::unique_ptr<chunk_base>> vector;
			try {
				construct_IHDR(span, vector);
			} catch (...) {
				should_fail = 0;
			}
			Assert::IsFalse(should_fail);
		}
		TEST_METHOD(construct_IHDR_test_5) {
			std::vector<std::uint8_t> data(13);
			std::span<std::uint8_t> span{ data.data(), data.data() + data.size() };
			std::uint8_t* position{ data.data() };
			bool should_fail{ 1 };
			write_4(18, position, span);
			write_4(424, position, span);
			write_1(16, position, span);
			write_1(4, position, span);
			write_1(0, position, span);
			write_1(4, position, span);
			std::vector<std::unique_ptr<chunk_base>> vector;
			try {
				construct_IHDR(span, vector);
			} catch (...) {
				should_fail = 0;
			}
			Assert::IsFalse(should_fail);
		}
		TEST_METHOD(construct_IHDR_test_6) {
			std::vector<std::uint8_t> data(13);
			std::span<std::uint8_t> span{ data.data(), data.data() + data.size() };
			std::uint8_t* position{ data.data() };
			bool should_fail{ 1 };
			write_4(18, position, span);
			write_4(424, position, span);
			write_1(16, position, span);
			write_1(4, position, span);
			write_1(0, position, span);
			write_1(0, position, span);
			write_1(4, position, span);
			std::vector<std::unique_ptr<chunk_base>> vector;
			try {
				construct_IHDR(span, vector);
			} catch (...) {
				should_fail = 0;
			}
			Assert::IsFalse(should_fail);
		}
		TEST_METHOD(construct_IHDR_test_7) {
			std::vector<std::uint8_t> data(12);
			std::span<std::uint8_t> span{ data.data(), data.data() + data.size() };
			std::uint8_t* position{ data.data() };
			bool should_fail{ 1 };
			write_4(499, position, span);
			write_4(424, position, span);
			write_1(16, position, span);
			write_1(4, position, span);
			std::vector<std::unique_ptr<chunk_base>> vector;
			try {
				construct_IHDR(span, vector);
			} catch (...) {
				should_fail = 0;
			}
			Assert::IsFalse(should_fail);
		}
		TEST_METHOD(construct_IHDR_test_8) {
			std::vector<std::uint8_t> data(14);
			std::span<std::uint8_t> span{ data.data(), data.data() + data.size() };
			std::uint8_t* position{ data.data() };
			bool should_fail{ 1 };
			write_4(499, position, span);
			write_4(424, position, span);
			write_1(16, position, span);
			write_1(4, position, span);
			std::vector<std::unique_ptr<chunk_base>> vector;
			try {
				construct_IHDR(span, vector);
			} catch (...) {
				should_fail = 0;
			}
			Assert::IsFalse(should_fail);
		}
		TEST_METHOD(construct_IHDR_test_9) {
			std::vector<std::uint8_t> data(13);
			std::span<std::uint8_t> span{ data.data(), data.data() + data.size() };
			std::uint8_t* position{ data.data() };
			write_4(234, position, span);
			write_4(424, position, span);
			write_1(16, position, span);
			write_1(4, position, span);
			std::vector<std::unique_ptr<chunk_base>> vector;
			vector.push_back(construct_IHDR(span, vector));
			bool should_fail{ 1 };
			try {
				construct_IHDR(span, vector);
			} catch (...) {
				should_fail = 0;
			}
			Assert::IsFalse(should_fail);
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
			Assert::IsFalse(should_fail);
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
			Assert::IsFalse(should_fail);
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
			Assert::IsFalse(should_fail);
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
			Assert::IsFalse(should_fail);
		}
	};
}