cmake_minimum_required(VERSION 3.21)

project(kernel CXX)

set(CMAKE_CXX_FLAGS "-ffreestanding -fbuiltin -fno-stack-protector -fno-exceptions -fno-rtti -fno-use-cxa-atexit -mno-red-zone -mcmodel=kernel")

set(CMAKE_EXE_LINKER_FLAGS "-lgcc -nostdlib -zmax-page-size=0x1000 -Tarch/${CMAKE_SYSTEM_PROCESSOR}/linker.lds")

file(GLOB_RECURSE CXX_SOURCES "*.cpp")

add_subdirectory(arch)

add_executable(kernel ${CXX_SOURCES})

target_compile_definitions(kernel
PRIVATE
    ARCH=${CMAKE_SYSTEM_PROCESSOR}
)

target_compile_options(kernel
PRIVATE
    $<$<COMPILE_LANGUAGE:CXX>:-Wall -Wextra -Werror -Wno-unused-parameter>
)

target_link_libraries(kernel arch)

set_target_properties(kernel
PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
    LINKER_LANGUAGE CXX
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)
