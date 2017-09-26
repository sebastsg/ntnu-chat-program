cmake_minimum_required(VERSION 3.6)

project(A4Client)

set(NOCTARE_ENGINE_DIR "${PROJECT_SOURCE_DIR}/../../../NoctareEngine")
set(A4_SHARED_DIR "${PROJECT_SOURCE_DIR}/../../A4Shared")
set(ROOT_DIR "${PROJECT_SOURCE_DIR}/../")

include_directories(
	${PROJECT_SOURCE_DIR}/../Include
	${NOCTARE_ENGINE_DIR}/Include
	${NOCTARE_ENGINE_DIR}/ThirdParty
	${A4_SHARED_DIR}/Include
)

if(CMAKE_CONFIGURATION_TYPES)
	set(CMAKE_CONFIGURATION_TYPES Debug Release)
	set(CMAKE_CONFIGURATION_TYPES "${CMAKE_CONFIGURATION_TYPES}" CACHE STRING "Reset configurations" FORCE)
endif()

file(GLOB_RECURSE SOURCE_FILES ${PROJECT_SOURCE_DIR}/../Source/*.cpp)
file(GLOB_RECURSE HEADER_FILES ${PROJECT_SOURCE_DIR}/../Include/*.hpp)

source_group("Source Files" FILES ${SOURCE_FILES})
source_group("Header Files" FILES ${HEADER_FILES})

add_executable(A4Client WIN32 ${SOURCE_FILES} ${HEADER_FILES})

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT A4Client)

if(${WIN32})
	set(DEBUG_LINK_LIBRARIES
		debug ${NOCTARE_ENGINE_DIR}/Libraries/debug/NoctareEngine.lib
		debug ${NOCTARE_ENGINE_DIR}/Libraries/debug/SDL2.lib
		debug ${NOCTARE_ENGINE_DIR}/Libraries/debug/SDL2main.lib
		debug ${NOCTARE_ENGINE_DIR}/Libraries/debug/SDL2_image.lib
		debug ${NOCTARE_ENGINE_DIR}/Libraries/debug/SDL2_mixer.lib
		debug ${NOCTARE_ENGINE_DIR}/Libraries/debug/SDL2_ttf.lib
		debug ${NOCTARE_ENGINE_DIR}/Libraries/debug/glew32d.lib
		debug ${A4_SHARED_DIR}/Libraries/debug/A4Shared.lib
		debug opengl32.lib
		debug glu32.lib
		debug ws2_32.lib
	)
	set(RELEASE_LINK_LIBRARIES
		optimized ${NOCTARE_ENGINE_DIR}/Libraries/release/NoctareEngine.lib
		optimized ${NOCTARE_ENGINE_DIR}/Libraries/release/SDL2.lib
		optimized ${NOCTARE_ENGINE_DIR}/Libraries/release/SDL2main.lib
		optimized ${NOCTARE_ENGINE_DIR}/Libraries/release/SDL2_image.lib
		optimized ${NOCTARE_ENGINE_DIR}/Libraries/release/SDL2_mixer.lib
		optimized ${NOCTARE_ENGINE_DIR}/Libraries/release/SDL2_ttf.lib
		optimized ${NOCTARE_ENGINE_DIR}/Libraries/release/glew32.lib
		optimized ${A4_SHARED_DIR}/Libraries/release/A4Shared.lib
		optimized opengl32.lib
		optimized glu32.lib
		optimized ws2_32.lib
	)
	set(ALL_LINK_LIBRARIES ${DEBUG_LINK_LIBRARIES} ${RELEASE_LINK_LIBRARIES})
	target_link_libraries(A4Client ${ALL_LINK_LIBRARIES})
	add_custom_command(TARGET A4Client PRE_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy "${NOCTARE_ENGINE_DIR}/Binaries/debug/NoctareEngine.dll" "${ROOT_DIR}/Development/NoctareEngine.dll"
		COMMAND ${CMAKE_COMMAND} -E copy "${A4_SHARED_DIR}/Binaries/debug/A4Shared.dll" "${ROOT_DIR}/Development/A4Shared.dll"
	)
endif()