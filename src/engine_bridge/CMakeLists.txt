# Engine Bridge - Adapters for CreateInterface and engine APIs

add_library(lib_engine_bridge STATIC
	# File system adapter
	filesystem_bridge.cpp
	filesystem_bridge.h

	# Material system adapter
	materialsystem_bridge.cpp
	materialsystem_bridge.h

	# Console/cvar adapter
	console_bridge.cpp
	console_bridge.h

	# TODO: Add more interface adapters as needed
)

target_include_directories(lib_engine_bridge PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_SOURCE_DIR}/src/public
	${CMAKE_SOURCE_DIR}/src/public/tier0
	${CMAKE_SOURCE_DIR}/src/public/tier1
	${CMAKE_SOURCE_DIR}/src/framework
)

target_link_libraries(lib_engine_bridge
	tier0
	vstdlib
	lib_framework
	# tier1 not needed yet (will add when we implement CreateInterface calls)
)
