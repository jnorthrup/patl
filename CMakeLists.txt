PROJECT(patl CXX)
CMAKE_MINIMUM_REQUIRED (VERSION 2.6)

# per https://stackoverflow.com/questions/7031126/switching-between-gcc-and-clang-llvm-using-cmake
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    # using Clang
    # $ cmake -D_CMAKE_TOOLCHAIN_PREFIX=llvm- ..
    SET (CMAKE_C_FLAGS_INIT                "-Wall -std=c99")
    SET (CMAKE_C_FLAGS_DEBUG_INIT          "-g")
    SET (CMAKE_C_FLAGS_MINSIZEREL_INIT     "-Os -DNDEBUG")
    SET (CMAKE_C_FLAGS_RELEASE_INIT        "-O4 -DNDEBUG")
    SET (CMAKE_C_FLAGS_RELWITHDEBINFO_INIT "-O2 -g")

    SET (CMAKE_CXX_FLAGS_INIT                "-Wall")
    SET (CMAKE_CXX_FLAGS_DEBUG_INIT          "-g")
    SET (CMAKE_CXX_FLAGS_MINSIZEREL_INIT     "-Os -DNDEBUG")
    SET (CMAKE_CXX_FLAGS_RELEASE_INIT        "-O4 -DNDEBUG")
    SET (CMAKE_CXX_FLAGS_RELWITHDEBINFO_INIT "-O2 -g")
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    # using GCC
    INCLUDE (CMakeMacro.cmake)
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Intel")
    # using Intel C++
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    # using Visual Studio C++
endif()
SET( CMAKE_CXX_STANDARD 14 )
option(USE_CLANG "build application with clang" OFF)

AUX_SOURCE_DIRECTORY(uxn/patl SRC_PATL)
ADD_LIBRARY(patl SHARED ${SRC_PATL})
INSTALL (TARGETS patl DESTINATION ${LIBDIR})

ADD_EXECUTABLE(patl_insider uxn/demos/insider/insider.cpp)
