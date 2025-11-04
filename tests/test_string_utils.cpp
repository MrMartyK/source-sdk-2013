//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Unit tests for framework string utilities
//
//=============================================================================

#include <catch2/catch_test_macros.hpp>
#include "string_utils.h"

using namespace S15;

TEST_CASE("StringCopy handles basic copying", "[string_utils]") {
	char dest[32];

	SECTION("Normal copy") {
		int copied = StringCopy(dest, "Hello", sizeof(dest));
		REQUIRE(copied == 5);
		REQUIRE(strcmp(dest, "Hello") == 0);
	}

	SECTION("Truncation") {
		int copied = StringCopy(dest, "This is a very long string that will be truncated", 10);
		REQUIRE(copied == 9);
		REQUIRE(strlen(dest) == 9);
		REQUIRE(dest[9] == '\0');
	}

	SECTION("Empty string") {
		int copied = StringCopy(dest, "", sizeof(dest));
		REQUIRE(copied == 0);
		REQUIRE(dest[0] == '\0');
	}

	SECTION("Null source") {
		int copied = StringCopy(dest, nullptr, sizeof(dest));
		REQUIRE(copied == 0);
	}
}

TEST_CASE("StringCompareI is case-insensitive", "[string_utils]") {
	REQUIRE(StringCompareI("hello", "HELLO") == 0);
	REQUIRE(StringCompareI("abc", "ABC") == 0);
	REQUIRE(StringCompareI("Test", "test") == 0);

	REQUIRE(StringCompareI("apple", "banana") < 0);
	REQUIRE(StringCompareI("zebra", "apple") > 0);
}

TEST_CASE("StringEndsWith detects suffixes", "[string_utils]") {
	SECTION("Case-sensitive") {
		REQUIRE(StringEndsWith("test.txt", ".txt") == true);
		REQUIRE(StringEndsWith("test.txt", ".TXT") == false);
		REQUIRE(StringEndsWith("filename.bsp", ".bsp") == true);
		REQUIRE(StringEndsWith("short", "longer_suffix") == false);
	}

	SECTION("Case-insensitive") {
		REQUIRE(StringEndsWithI("test.TXT", ".txt") == true);
		REQUIRE(StringEndsWithI("MODEL.MDL", ".mdl") == true);
		REQUIRE(StringEndsWithI("SOUND.WAV", ".wav") == true);
	}
}

TEST_CASE("GetFileExtension extracts extensions", "[string_utils]") {
	REQUIRE(strcmp(GetFileExtension("test.txt"), "txt") == 0);
	REQUIRE(strcmp(GetFileExtension("model.mdl"), "mdl") == 0);
	REQUIRE(strcmp(GetFileExtension("path/to/file.bsp"), "bsp") == 0);
	REQUIRE(strcmp(GetFileExtension("path\\to\\file.vtf"), "vtf") == 0);

	// No extension
	REQUIRE(strcmp(GetFileExtension("noext"), "") == 0);
	REQUIRE(strcmp(GetFileExtension("path/noext"), "") == 0);

	// Dot in directory name, not file
	REQUIRE(strcmp(GetFileExtension("path.dir/noext"), "") == 0);
}

TEST_CASE("GetFilename strips directories", "[string_utils]") {
	REQUIRE(strcmp(GetFilename("test.txt"), "test.txt") == 0);
	REQUIRE(strcmp(GetFilename("path/to/file.txt"), "file.txt") == 0);
	REQUIRE(strcmp(GetFilename("path\\to\\file.txt"), "file.txt") == 0);
	REQUIRE(strcmp(GetFilename("C:\\Windows\\System32\\file.dll"), "file.dll") == 0);

	// No directory
	REQUIRE(strcmp(GetFilename("standalone"), "standalone") == 0);
}
