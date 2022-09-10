cmake_minimum_required(VERSION 3.21)

project(kernel ASM_NASM C CXX)

set(CMAKE_CXX_FLAGS "-ffreestanding -fbuiltin -fno-stack-protector -fno-exceptions -fno-rtti -mno-red-zone -mcmodel=kernel")

file(GLOB_RECURSE ASM_SOURCES "arch/${CMAKE_SYSTEM_PROCESSOR}/*.asm")
file(GLOB_RECURSE CXX_SOURCES "arch/${CMAKE_SYSTEM_PROCESSOR}/*.cpp" "*.cpp")

include_directories("include" "include/${CMAKE_SYSTEM_PROCESSOR}")

add_executable(kernel.elf ${ASM_SOURCES} ${CXX_SOURCES})

target_compile_options(kernel.elf
PRIVATE
    $<$<COMPILE_LANGUAGE:CXX>:-Wall -Wextra -Werror -Wno-unused-parameter>
)

set_target_properties(kernel.elf 
PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
    LINKER_LANGUAGE CXX
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)
