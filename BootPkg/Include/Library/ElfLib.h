#pragma once

#include <Uefi.h>

typedef VOID* ElfImageExecute(VOID* data) __attribute__((sysv_abi));

typedef struct ElfImage {
    VOID* entry;
    VOID* start;
    UINTN pageCount;
    ElfImageExecute* Execute;
} ElfImage;

/// @brief Load ELF image from disk to memory.
/// @param path path to ELF image file.
/// @param handle EFI application image handle.
/// @param image pointer to ELF image in memory.
/// @return EFI status code.
EFI_STATUS LoadElfImage(ElfImage* image, EFI_HANDLE handle, CHAR16* path);
