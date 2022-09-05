cmake_minimum_required(VERSION 3.21)

project(loader ASM_NASM)

file(GLOB_RECURSE ASM_SOURCES "arch/${CMAKE_SYSTEM_PROCESSOR}/*.asm")

add_executable(loader.elf ${ASM_SOURCES})

set_target_properties(loader.elf 
PROPERTIES
    LINKER_LANGUAGE C
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)
