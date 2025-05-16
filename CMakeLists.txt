cmake_minimum_required(VERSION 3.10)

project(SOI_Virtual_Drive VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
#set(CMAKE_COMPILE_WARNING_AS_ERROR ON)

FILE(GLOB_RECURSE SRC "src/*.c")


add_executable(SOI_Virtual_Drive main.c ${SRC})


target_include_directories(SOI_Virtual_Drive PRIVATE include)

if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(SOI_Virtual_Drive PRIVATE -Wall -Wextra -Wpedantic -Wunused-variable)
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    target_compile_options(SOI_Virtual_Drive PRIVATE /W4)
endif()

install(TARGETS SOI_Virtual_Drive DESTINATION bin)
