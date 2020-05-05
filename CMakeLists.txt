cmake_minimum_required(VERSION 2.8.5)
project(MemSnap LANGUAGES C)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

if(NOT AMIGA)
	message(SEND_ERROR "This project only compiles for Amiga")
endif()

set(CMAKE_C_STANDARD 11)
file(GLOB_RECURSE SOURCES source/*.c source/*.h)
add_executable(MemSnap ${SOURCES})
target_include_directories(MemSnap PRIVATE ${PROJECT_SOURCE_DIR}/source)
target_compile_options(MemSnap PUBLIC -Wall -Wextra -Os)
