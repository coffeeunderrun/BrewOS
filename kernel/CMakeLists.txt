project(Kernel VERSION 0.1 LANGUAGES C)

set(CMAKE_C_FLAGS "-mno-red-zone -mcmodel=kernel -ffreestanding -fbuiltin -fvisibility=hidden \
    -fomit-frame-pointer -fno-exceptions -fno-asynchronous-unwind-tables -fno-unwind-tables -fno-stack-protector")

set(CMAKE_EXE_LINKER_FLAGS "-lgcc -nostdlib -zmax-page-size=0x1000 -T${CMAKE_CURRENT_SOURCE_DIR}/arch/${CMAKE_SYSTEM_PROCESSOR}/linker.lds")

add_subdirectory(arch)

file(GLOB SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/*.c
)

file(GLOB HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/*.h
)

file(GLOB RESOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/resource/*.o
)

add_executable(${PROJECT_NAME}
    ${SOURCES}
    ${HEADERS}
    ${RESOURCES}
)

target_compile_options(${PROJECT_NAME}
    PRIVATE
    $<$<COMPILE_LANGUAGE:C>:-Wall -Wextra -Werror -Wno-unused-parameter>
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
    Arch
)

set_target_properties(${PROJECT_NAME}
    PROPERTIES
    C_STANDARD 23
    C_STANDARD_REQUIRED YES
    C_EXTENSIONS NO
    LINKER_LANGUAGE C
)
