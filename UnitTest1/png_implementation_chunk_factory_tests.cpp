#include "pch.h"
#include "png_implementation_chunk_factory.hpp"
#include <CppUnitTest.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace png_implementation_chunk_factory_tests {
	TEST_CLASS(crc32_tests) {
	public:
		TEST_METHOD(crc32_test_1) {
			std::uint8_t arr[]{ 'a', 'b', 'c', 'd', 'e', 'f' };
			Assert::IsTrue(0x4b8e39ef == crc32({ arr, arr + 6 }));
		}
		TEST_METHOD(crc32_test_2) {
			std::uint8_t buffer;
			Assert::IsTrue(0 == crc32({ &buffer, &buffer }));
		}
	};
	TEST_CLASS(is_valid_chunk_type_tests) {
	public:
		TEST_METHOD(is_valid_chunk_type_test_1) {
			std::uint_fast32_t arr[]{ 'a', 'b', 'c', 'd' }, acc{ 0 };
			for (auto i : arr) {
				acc <<= 8;
				acc |= i;
			}
			Assert::IsTrue(is_valid_chunk_type(acc));
		}
		TEST_METHOD(is_valid_chunk_type_test_2) {
			std::uint_fast32_t arr[]{ 'a', 'B', 'C', 'd' }, acc{ 0 };
			for (auto i : arr) {
				acc <<= 8;
				acc |= i;
			}
			Assert::IsTrue(is_valid_chunk_type(acc));
		}
		TEST_METHOD(is_valid_chunk_type_test_3) {
			std::uint_fast32_t arr[]{ '\n', 'b', 'c', 'd' }, acc{ 0 };
			for (auto i : arr) {
				acc <<= 8;
				acc |= i;
			}
			Assert::IsFalse(is_valid_chunk_type(acc));
		}
		TEST_METHOD(is_valid_chunk_type_test_4) {
			std::uint_fast32_t arr[]{ 'A', 'z' + 1, 'c', 'd' }, acc{ 0 };
			for (auto i : arr) {
				acc <<= 8;
				acc |= i;
			}
			Assert::IsFalse(is_valid_chunk_type(acc));
		}
		TEST_METHOD(is_valid_chunk_type_test_5) {
			std::uint_fast32_t arr[]{ 'Z' + 1, 'b', 'c', 'd' }, acc{ 0 };
			for (auto i : arr) {
				acc <<= 8;
				acc |= i;
			}
			Assert::IsFalse(is_valid_chunk_type(acc));
		}
	};
	TEST_CLASS(is_critical_chunk_type_tests) {
	public:
		TEST_METHOD(is_critical_chunk_type_test_1) {
			std::uint_fast32_t arr[]{ 'a', 'b', 'c', 'd' }, acc{ 0 };
			for (auto i : arr) {
				acc <<= 8;
				acc |= i;
			}
			Assert::IsFalse(is_critical_chunk_type(acc));
		}
		TEST_METHOD(is_critical_chunk_type_test_2) {
			std::uint_fast32_t arr[]{ 'C', 'b', 'c', 'd' }, acc{ 0 };
			for (auto i : arr) {
				acc <<= 8;
				acc |= i;
			}
			Assert::IsTrue(is_critical_chunk_type(acc));
		}
	};
	TEST_CLASS(reserved_bit_set_tests) {
	public:
		TEST_METHOD(reserved_bit_set_test_1) {
			std::uint_fast32_t arr[]{ 'a', 'b', 'C', 'd' }, acc{ 0 };
			for (auto i : arr) {
				acc <<= 8;
				acc |= i;
			}
			Assert::IsFalse(reserved_bit_set(acc));
		}
		TEST_METHOD(reserved_bit_set_test_2) {
			std::uint_fast32_t arr[]{ 'a', 'b', 'c', 'd' }, acc{ 0 };
			for (auto i : arr) {
				acc <<= 8;
				acc |= i;
			}
			Assert::IsTrue(reserved_bit_set(acc));
		}
	};
	TEST_CLASS(assert_can_read_tests) {
	public:
		TEST_METHOD(assert_can_read_test_1) {
			std::uint8_t buffer[100]{};
			assert_can_read(buffer + 25, { buffer, buffer + 100 });
		}
		TEST_METHOD(assert_can_read_test_2) {
			std::uint8_t buffer[100]{};
			assert_can_read(buffer + 99, { buffer, buffer + 100 });
		}
		TEST_METHOD(assert_can_read_test_3) {
			std::uint8_t buffer[100]{};
			bool should_fail{ 1 };
			try {
				assert_can_read(buffer + 100, { buffer, buffer + 100 });
			} catch (...) {
				should_fail = 0;
			}
			Assert::IsFalse(should_fail);
		}
	};
	TEST_CLASS(read_4_tests) {
	public:
		TEST_METHOD(read_4_test_1) {
			std::uint8_t buffer[]{ 1, 2, 3, 4 };
			const std::uint8_t* position{ buffer };
			std::uint_fast32_t acc{ 0 };
			for (auto i : buffer) {
				acc <<= 8;
				acc |= i;
			}
			Assert::IsTrue(read_4(position, { buffer, buffer + 4 }) == acc);
			Assert::IsTrue(position == buffer + 4);
		}
		TEST_METHOD(read_4_test_2) {
			std::uint8_t buffer[4];
			const std::uint8_t* position{ buffer };
			bool should_fail{ 1 };
			try {
				read_4(position, { buffer, buffer + 3 });
			} catch (...) {
				should_fail = 0;
			}
			Assert::IsFalse(should_fail);
		}
	};
	TEST_CLASS(write_4_tests) {
	public:
		TEST_METHOD(write_4_test_1) {
			std::uint8_t buffer[4], * position{ buffer };
			write_4(0xfa05'1034, position, { buffer, buffer + 4 });
			position -= 4;
			const std::uint8_t* pos{ position };
			Assert::IsTrue(read_4(pos, { buffer, buffer + 4 }) == 0xfa05'1034);
		}
		TEST_METHOD(write_4_test_2) {
			std::uint8_t buffer[4], *position{ buffer };
			bool should_fail{ 1 };
			try {
				write_4(0, position, { buffer, buffer + 2 });
			} catch (...) {
				should_fail = 0;
			}
			Assert::IsFalse(should_fail);
		}
	};
	TEST_CLASS(read_8_tests) {
	public:
		TEST_METHOD(read_8_test_1) {
			std::uint8_t buffer[]{ 1, 2, 3, 4, 8, 37, 5, 6 };
			const std::uint8_t* position{ buffer };
			std::uint_fast64_t acc{ 0 };
			for (auto i : buffer) {
				acc <<= 8;
				acc |= i;
			}
			Assert::IsTrue(read_8(position, { buffer, buffer + 8 }) == acc);
			Assert::IsTrue(position == buffer + 8);
		}
		TEST_METHOD(read_8_test_2) {
			std::uint8_t buffer[8];
			const std::uint8_t* position{ buffer };
			bool should_fail{ 1 };
			try {
				read_8(position, { buffer, buffer + 4 });
			} catch (...) {
				should_fail = 0;
			}
			Assert::IsFalse(should_fail);
		}
	};
	TEST_CLASS(write_8_tests) {
	public:
		TEST_METHOD(write_8_test_1) {
			std::uint8_t buffer[8], * position{ buffer };
			write_8(0xfa05'1034'3322'0034, position, { buffer, buffer + 8 });
			position -= 8;
			const std::uint8_t* pos{ position };
			Assert::IsTrue(read_8(pos, { buffer, buffer + 8 }) == 0xfa05'1034'3322'0034);
		}
		TEST_METHOD(write_8_test_2) {
			std::uint8_t buffer[8], * position{ buffer };
			bool should_fail{ 1 };
			try {
				write_8(0, position, { buffer, buffer + 2 });
			} catch (...) {
				should_fail = 0;
			}
			Assert::IsFalse(should_fail);
		}
	};
	TEST_CLASS(read_1_tests) {
	public:
		TEST_METHOD(read_1_test_1) {
			std::uint8_t buffer[]{ 1 };
			const std::uint8_t* position{ buffer };
			std::uint_fast64_t acc{ 1 };
			Assert::IsTrue(read_1(position, { buffer, buffer + 1 }) == acc);
			Assert::IsTrue(position == buffer + 1);
		}
		TEST_METHOD(read_1_test_2) {
			std::uint8_t buffer[8];
			const std::uint8_t* position{ buffer };
			bool should_fail{ 1 };
			try {
				read_1(position, { buffer, buffer });
			} catch (...) {
				should_fail = 0;
			}
			Assert::IsFalse(should_fail);
		}
	};
	TEST_CLASS(write_1_tests) {
	public:
		TEST_METHOD(write_1_test_1) {
			std::uint8_t buffer[1], * position{ buffer };
			write_1(0x34, position, { buffer, buffer + 1 });
			--position;
			const std::uint8_t* pos{ position };
			Assert::IsTrue(read_1(pos, { buffer, buffer + 1 }) == 0x34);
		}
		TEST_METHOD(write_1_test_2) {
			std::uint8_t buffer[1], * position{ buffer };
			bool should_fail{ 1 };
			try {
				write_1(0, position, { buffer, buffer });
			} catch (...) {
				should_fail = 0;
			}
			Assert::IsFalse(should_fail);
		}
	};
}