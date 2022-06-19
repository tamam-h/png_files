#include "pch.h"
#include "png_implementation_zlib_stream.hpp"
#include <CppUnitTest.h>
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace png_implementation_zlib_stream_tests {
	TEST_CLASS(adler32_tests) {
	public:
		TEST_METHOD(adler32_test_1) {
			const std::uint8_t buffer[]{ "1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18" };
			Assert::AreEqual(static_cast<uint_fast32_t>(3271624791), adler32({ buffer, buffer + sizeof(buffer) }));
		}
		TEST_METHOD(adler32_test_2) {
			const std::uint8_t buffer[]{ "1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34" };
			Assert::AreEqual(static_cast<uint_fast32_t>(1003032955), adler32({ buffer, buffer + sizeof(buffer) }));
		}
	};
	TEST_CLASS(zlib_stream_handler_tests) {
	public:
		TEST_METHOD(zlib_stream_handler_test_1) {
			std::vector<std::uint8_t> data;
			data.resize(100000);
			for (auto& i : data) { i = std::rand() % UINT8_MAX; }
			zlib_stream_handler handler{ .uncompressed_data{ data } };
			handler.compress();
			Assert::IsTrue(data == handler.uncompressed_data);
			handler.uncompressed_data.push_back('a');
			handler.decompress();
			Assert::IsTrue(data == handler.uncompressed_data);
		}
	};
}