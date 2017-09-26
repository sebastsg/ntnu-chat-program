cmake_minimum_required(VERSION 3.6)

project(A4Shared)

set(ROOT_DIR "${PROJECT_SOURCE_DIR}/../")
set(NOCTARE_ENGINE_DIR "${PROJECT_SOURCE_DIR}/../../../NoctareEngine")

message("Configuring the environment")
include_directories(
	${ROOT_DIR}/Include
	${NOCTARE_ENGINE_DIR}/Include
	${NOCTARE_ENGINE_DIR}/ThirdParty
)

if(CMAKE_CONFIGURATION_TYPES)
	set(CMAKE_CONFIGURATION_TYPES Debug Release)
	set(CMAKE_CONFIGURATION_TYPES "${CMAKE_CONFIGURATION_TYPES}" CACHE STRING "Reset configurations" FORCE)
	message("Configuration types: ${CMAKE_CONFIGURATION_TYPES}")
endif()

file(GLOB_RECURSE SOURCE_FILES ${ROOT_DIR}/Source/*.cpp)

file(GLOB_RECURSE HEADER_FILES ${ROOT_DIR}/Include/*.hpp)

add_library(A4Shared SHARED ${SOURCE_FILES} ${HEADER_FILES})

if(${WIN32})
	set(DEBUG_LINK_LIBRARIES 
		debug ${NOCTARE_ENGINE_DIR}/Libraries/debug/NoctareEngine.lib
		debug ${NOCTARE_ENGINE_DIR}/Libraries/debug/SDL2.lib
		debug ${NOCTARE_ENGINE_DIR}/Libraries/debug/SDL2_image.lib
		debug ${NOCTARE_ENGINE_DIR}/Libraries/debug/SDL2_mixer.lib
		debug ${NOCTARE_ENGINE_DIR}/Libraries/debug/SDL2_ttf.lib
		debug ${NOCTARE_ENGINE_DIR}/Libraries/debug/glew32d.lib
		debug opengl32.lib
		debug glu32.lib
		debug ws2_32.lib
	)
	set(RELEASE_LINK_LIBRARIES
		debug ${NOCTARE_ENGINE_DIR}/Libraries/debug/NoctareEngine.lib
		optimized ${NOCTARE_ENGINE_DIR}/Libraries/release/SDL2.lib
		optimized ${NOCTARE_ENGINE_DIR}/Libraries/release/SDL2_image.lib
		optimized ${NOCTARE_ENGINE_DIR}/Libraries/release/SDL2_mixer.lib
		optimized ${NOCTARE_ENGINE_DIR}/Libraries/release/SDL2_ttf.lib
		optimized ${NOCTARE_ENGINE_DIR}/Libraries/release/glew32.lib
		optimized opengl32.lib
		optimized glu32.lib
		optimized ws2_32.lib
	)
	set(ALL_LINK_LIBRARIES ${DEBUG_LINK_LIBRARIES} ${RELEASE_LINK_LIBRARIES})
	target_link_libraries(A4Shared ${ALL_LINK_LIBRARIES})
endif()

add_custom_command(TARGET A4Shared POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy "$<LOWER_CASE:$<CONFIG>>/A4Shared.lib" "${ROOT_DIR}/Libraries/$<LOWER_CASE:$<CONFIG>>/A4Shared.lib"
	COMMAND ${CMAKE_COMMAND} -E copy "$<LOWER_CASE:$<CONFIG>>/A4Shared.dll" "${ROOT_DIR}/Binaries/$<LOWER_CASE:$<CONFIG>>/A4Shared.dll"
)