# Framework library - engine-agnostic utilities for Source 1.5

add_library(lib_framework STATIC
	# String utilities
	string_utils.cpp
	string_utils.h

	# Math helpers (beyond mathlib)
	math_extra.cpp
	math_extra.h

	# Serialization
	serialization.cpp
	serialization.h

	# TODO: Add more as we extract from game/shared
)

target_include_directories(lib_framework PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_SOURCE_DIR}/src/public
	${CMAKE_SOURCE_DIR}/src/public/tier0
	${CMAKE_SOURCE_DIR}/src/public/tier1
	${CMAKE_SOURCE_DIR}/src/public/mathlib
)

# Framework is engine-agnostic - no tier library dependencies
# (only standard C/C++ library)
# target_link_libraries(lib_framework
# 	tier0
# 	vstdlib
# )

# Install headers for other modules
install(
	DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/
	DESTINATION include/framework
	FILES_MATCHING PATTERN "*.h"
)
