//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Advanced unit tests for framework string utilities
//          Testing edge cases, performance, and boundary conditions
//
//=============================================================================

#include <catch2/catch_test_macros.hpp>
#include "string_utils.h"
#include <string.h>

using namespace S15;

TEST_CASE("StringCopy handles buffer boundaries", "[string_utils][edge-cases]") {
	char dest[10];

	SECTION("Exactly at buffer limit") {
		// 9 chars + null terminator = exactly buffer size
		int copied = StringCopy(dest, "123456789", sizeof(dest));
		REQUIRE(copied == 9);
		REQUIRE(dest[9] == '\0');
		REQUIRE(strcmp(dest, "123456789") == 0);
	}

	SECTION("One over buffer limit") {
		// 10 chars should truncate to 9
		int copied = StringCopy(dest, "1234567890", sizeof(dest));
		REQUIRE(copied == 9);
		REQUIRE(dest[9] == '\0');
		REQUIRE(strlen(dest) == 9);
	}

	SECTION("Buffer size of 1 (edge case)") {
		char tiny[1];
		int copied = StringCopy(tiny, "anything", sizeof(tiny));
		REQUIRE(copied == 0);
		REQUIRE(tiny[0] == '\0');
	}

	SECTION("Buffer size of 2 (one char + null)") {
		char small[2];
		int copied = StringCopy(small, "Hello", sizeof(small));
		REQUIRE(copied == 1);
		REQUIRE(small[0] == 'H');
		REQUIRE(small[1] == '\0');
	}
}

TEST_CASE("StringCopy handles invalid inputs", "[string_utils][safety]") {
	char dest[32];

	SECTION("Null destination") {
		int copied = StringCopy(nullptr, "test", 32);
		REQUIRE(copied == 0);
	}

	SECTION("Null source") {
		int copied = StringCopy(dest, nullptr, sizeof(dest));
		REQUIRE(copied == 0);
	}

	SECTION("Both null") {
		int copied = StringCopy(nullptr, nullptr, 32);
		REQUIRE(copied == 0);
	}

	SECTION("Zero buffer size") {
		int copied = StringCopy(dest, "test", 0);
		REQUIRE(copied == 0);
	}

	SECTION("Negative buffer size") {
		int copied = StringCopy(dest, "test", -1);
		REQUIRE(copied == 0);
	}
}

TEST_CASE("StringCompareI handles special characters", "[string_utils][edge-cases]") {
	SECTION("Numbers are case-insensitive (trivially)") {
		REQUIRE(StringCompareI("123", "123") == 0);
		REQUIRE(StringCompareI("999", "000") > 0);
	}

	SECTION("Mixed alphanumeric") {
		REQUIRE(StringCompareI("Test123", "test123") == 0);
		REQUIRE(StringCompareI("ABC123", "abc123") == 0);
	}

	SECTION("Special characters") {
		REQUIRE(StringCompareI("hello_world", "HELLO_WORLD") == 0);
		REQUIRE(StringCompareI("test-file", "TEST-FILE") == 0);
		REQUIRE(StringCompareI("path/to/file", "PATH/TO/FILE") == 0);
	}

	SECTION("Leading/trailing differences") {
		REQUIRE(StringCompareI(" test", "test") < 0); // Space comes before 't'
		REQUIRE(StringCompareI("test ", "test") > 0); // Space after
	}
}

TEST_CASE("GetFileExtension handles complex paths", "[string_utils][paths]") {
	SECTION("Multiple dots in filename") {
		REQUIRE(strcmp(GetFileExtension("archive.tar.gz"), "gz") == 0);
		REQUIRE(strcmp(GetFileExtension("file.backup.txt"), "txt") == 0);
	}

	SECTION("Hidden files (Unix)") {
		REQUIRE(strcmp(GetFileExtension(".gitignore"), "gitignore") == 0);
		REQUIRE(strcmp(GetFileExtension(".hidden"), "hidden") == 0);
	}

	SECTION("Dot at start of filename") {
		REQUIRE(strcmp(GetFileExtension("path/to/.hidden"), "hidden") == 0);
	}

	SECTION("Multiple directory levels") {
		REQUIRE(strcmp(GetFileExtension("C:/Program Files/Steam/game.exe"), "exe") == 0);
		REQUIRE(strcmp(GetFileExtension("/usr/local/bin/tool.sh"), "sh") == 0);
	}

	SECTION("Very long paths") {
		const char* longPath = "very/long/path/with/many/directories/and/subdirectories/file.bsp";
		REQUIRE(strcmp(GetFileExtension(longPath), "bsp") == 0);
	}

	SECTION("Empty string") {
		REQUIRE(strcmp(GetFileExtension(""), "") == 0);
	}

	SECTION("Just a dot") {
		REQUIRE(strcmp(GetFileExtension("."), "") == 0);
	}

	SECTION("Trailing dot (no extension)") {
		REQUIRE(strcmp(GetFileExtension("file."), "") == 0);
	}
}

TEST_CASE("GetFilename handles edge cases", "[string_utils][paths]") {
	SECTION("Root directory paths") {
		REQUIRE(strcmp(GetFilename("C:\\"), "") == 0);
		REQUIRE(strcmp(GetFilename("/"), "") == 0);
	}

	SECTION("Trailing slashes") {
		REQUIRE(strcmp(GetFilename("path/to/dir/"), "") == 0);
		REQUIRE(strcmp(GetFilename("path\\to\\dir\\"), "") == 0);
	}

	SECTION("Mixed slashes (Windows)") {
		REQUIRE(strcmp(GetFilename("C:/Windows\\System32/file.dll"), "file.dll") == 0);
	}

	SECTION("UNC paths") {
		REQUIRE(strcmp(GetFilename("\\\\server\\share\\file.txt"), "file.txt") == 0);
	}

	SECTION("Empty string") {
		REQUIRE(strcmp(GetFilename(""), "") == 0);
	}

	SECTION("Just slash") {
		REQUIRE(strcmp(GetFilename("/"), "") == 0);
		REQUIRE(strcmp(GetFilename("\\"), "") == 0);
	}
}

TEST_CASE("StringEndsWith handles empty strings", "[string_utils][edge-cases]") {
	SECTION("Empty string and empty suffix") {
		REQUIRE(StringEndsWith("", "") == true);
	}

	SECTION("Non-empty string and empty suffix") {
		REQUIRE(StringEndsWith("test", "") == true);
	}

	SECTION("Empty string and non-empty suffix") {
		REQUIRE(StringEndsWith("", ".txt") == false);
	}
}

TEST_CASE("String functions handle very long inputs", "[string_utils][performance]") {
	// Create a long string (1KB)
	char longSource[1024];
	char dest[1024];

	for (int i = 0; i < 1023; i++) {
		longSource[i] = 'a' + (i % 26);
	}
	longSource[1023] = '\0';

	SECTION("StringCopy with large buffer") {
		int copied = StringCopy(dest, longSource, sizeof(dest));
		REQUIRE(copied == 1023);
		REQUIRE(strcmp(dest, longSource) == 0);
	}

	SECTION("StringCompareI with long strings") {
		// Create uppercase version
		char longUpper[1024];
		for (int i = 0; i < 1023; i++) {
			longUpper[i] = 'A' + (i % 26);
		}
		longUpper[1023] = '\0';

		REQUIRE(StringCompareI(longSource, longUpper) == 0);
	}
}
