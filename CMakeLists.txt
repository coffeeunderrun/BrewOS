cmake_minimum_required(VERSION 3.21)

include(toolchain.cmake)

project(brewos VERSION 1.0 LANGUAGES ASM_NASM C CXX)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

set(CMAKE_ASM_NASM_FLAGS_DEBUG "-gdwarf -O0")
set(CMAKE_ASM_NASM_FLAGS_RELEASE "-Ox")

set(CMAKE_C_FLAGS_DEBUG "-gdwarf-2 -O0")
set(CMAKE_C_FLAGS_RELEASE "-O3")

set(CMAKE_CXX_FLAGS_DEBUG "-gdwarf-2 -O0")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

set(CMAKE_EXE_LINKER_FLAGS "-lgcc -nostdlib -zmax-page-size=0x1000 -Tarch/${CMAKE_SYSTEM_PROCESSOR}/linker.lds")
set(CMAKE_EXE_LINKER_FLAGS_DEBUG "")
set(CMAKE_EXE_LINKER_FLAGS_RELEASE "-s")

file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

add_subdirectory(kernel)
