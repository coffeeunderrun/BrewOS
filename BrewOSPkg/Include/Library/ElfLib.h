#ifndef ELFLIB_H
#define ELFLIB_H

#include <Uefi.h>

typedef struct ElfImage
{
    VOID *entry;
    VOID *top;
    UINTN pages;
} ElfImage;

EFI_STATUS LoadElfImage(CHAR16 *fileName, EFI_HANDLE handle, ElfImage *image);

#endif // ELFLIB_H
