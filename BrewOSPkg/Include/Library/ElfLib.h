#ifndef ELFLIB_H
#define ELFLIB_H

EFI_STATUS LoadElfImage(CHAR16 *fileName, EFI_HANDLE handle, VOID **entry, VOID **top, UINTN *pages);

#endif // ELFLIB_H