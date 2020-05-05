cmake_minimum_required(VERSION 2.8.5)
project(MemSnap LANGUAGES C)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

if(NOT AMIGA)
	message(SEND_ERROR "This project only compiles for Amiga")
endif()

set(CMAKE_C_STANDARD 11)
file(GLOB_RECURSE SOURCES source/*.c source/*.h)
add_executable(${PROJECT_NAME} ${SOURCES})
target_include_directories(${PROJECT_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/source)
target_compile_options(${PROJECT_NAME} PUBLIC -Wall -Wextra -Os -msmall-code -mregparm=2 -fbaserel -ffast-math -s -mcrt=nix20)
target_link_options(${PROJECT_NAME} PUBLIC -mregparm=2 -fbaserel -ffast-math -s -mcrt=nix20)

set(MAJOR_VERSION 1)
set(MINOR_VERSION 2)
set(AMIGA_VERSION "${MAJOR_VERSION}.${MINOR_VERSION}")

configure_file(
		${PROJECT_SOURCE_DIR}/source/version.h.in
		version.h
)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

target_link_options(${PROJECT_NAME} PRIVATE -Wa,--base-size-default-16,-smallcode -Wl,-Map=MemSnap.map)
