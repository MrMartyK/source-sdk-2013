# Source 1.5 - Modern CMake build system
# Coexists with VPC for compatibility during migration

cmake_minimum_required(VERSION 3.20)

project(Source15
	VERSION 0.1.0
	LANGUAGES CXX C
)

# Options
option(SOURCE15_BUILD_TESTS "Build unit tests" ON)
option(SOURCE15_STEAMAUDIO "Enable Steam Audio integration" OFF)
option(SOURCE15_BUILD_DX11 "Build DX11 renderer module" OFF)
option(SOURCE15_IWYU "Run include-what-you-use" OFF)

# C++17 required for Source 1.5 features
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Export compile commands for clangd/IDE integration
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Platform detection
if(WIN32)
	set(PLATFORM_WINDOWS TRUE)
	add_definitions(-DPLATFORM_WINDOWS -D_WIN32)
elseif(UNIX)
	set(PLATFORM_LINUX TRUE)
	add_definitions(-DPLATFORM_LINUX -DPOSIX)
endif()

# Disable exceptions and RTTI (Source SDK convention)
if(MSVC)
	add_compile_options(
		/W3                    # Warning level 3
		/WX-                   # Warnings not as errors (for now)
		/GR-                   # Disable RTTI
		/EHsc                  # Exception handling (minimal)
		/MP                    # Multi-processor compilation
		/Zi                    # Debug info
		/fp:fast               # Fast floating point
	)
	add_definitions(
		-D_CRT_SECURE_NO_WARNINGS
		-D_CRT_NONSTDC_NO_DEPRECATE
	)
else()
	add_compile_options(
		-Wall
		-Wno-unused-variable
		-fno-rtti
		-fno-exceptions
		-ffast-math
	)
endif()

# Source SDK include paths (tier system)
include_directories(
	${CMAKE_SOURCE_DIR}/src/public
	${CMAKE_SOURCE_DIR}/src/public/tier0
	${CMAKE_SOURCE_DIR}/src/public/tier1
	${CMAKE_SOURCE_DIR}/src/public/mathlib
	${CMAKE_SOURCE_DIR}/src/common
)

# Link directories for precompiled libs
link_directories(
	${CMAKE_SOURCE_DIR}/src/lib/public/x64
)

# Subdirectories (new modular structure)
# NOTE: tier0/tier1/mathlib remain as precompiled .lib files for now
# We only build new modules here

# Framework - new engine-agnostic utilities
add_subdirectory(src/framework)

# Engine bridge - adapters for CreateInterface
add_subdirectory(src/engine_bridge)

# Material system additions (PBR shader)
if(SOURCE15_BUILD_DX11)
	add_subdirectory(src/materialsystem/shaderapidx11)
endif()

# Steam Audio integration (optional)
if(SOURCE15_STEAMAUDIO)
	add_subdirectory(src/soundemittersystem/steamaudio)
endif()

# Tests
if(SOURCE15_BUILD_TESTS)
	enable_testing()
	add_subdirectory(tests)
endif()

# Tools (Hub will be separate repo, but utilities go here)
add_subdirectory(src/tools)

# Print configuration summary
message(STATUS "")
message(STATUS "Source 1.5 Configuration:")
message(STATUS "  Version: ${PROJECT_VERSION}")
message(STATUS "  Platform: ${CMAKE_SYSTEM_NAME}")
message(STATUS "  Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "  Tests: ${SOURCE15_BUILD_TESTS}")
message(STATUS "  Steam Audio: ${SOURCE15_STEAMAUDIO}")
message(STATUS "  DX11 Renderer: ${SOURCE15_BUILD_DX11}")
message(STATUS "  IWYU: ${SOURCE15_IWYU}")
message(STATUS "")
