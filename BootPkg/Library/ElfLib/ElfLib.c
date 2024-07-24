#include <Common.h>
#include <elf.h>
#include <Library/DebugLib.h>
#include <Library/ElfLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiLib.h>
#include <Protocol/LoadedImage.h>
#include <Protocol/SimpleFileSystem.h>

static EFI_STATUS LoadElfHeader(EFI_FILE*, VOID*, VOID**);
static EFI_STATUS LoadElf32Image(EFI_FILE*, Elf32_Ehdr*, VOID**, UINTN*);
static EFI_STATUS LoadElf64Image(EFI_FILE*, Elf64_Ehdr*, VOID**, UINTN*);

EFI_STATUS LoadElfImage(ElfImage* image, EFI_HANDLE handle, CHAR16* path){
    ASSERT(path != NULL);
    ASSERT(image != NULL);

    EFI_STATUS status;

    EFI_LOADED_IMAGE_PROTOCOL* loadedImage;
    status = gBS->HandleProtocol(handle, &gEfiLoadedImageProtocolGuid, (VOID**)&loadedImage);
    RETURN_IF_ERROR_STATUS(status);

    EFI_SIMPLE_FILE_SYSTEM_PROTOCOL* fileSystem;
    status = gBS->HandleProtocol(loadedImage->DeviceHandle, &gEfiSimpleFileSystemProtocolGuid, (VOID**)&fileSystem);
    RETURN_IF_ERROR_STATUS(status);

    EFI_FILE* root;
    status = fileSystem->OpenVolume(fileSystem, &root);
    RETURN_IF_ERROR_STATUS(status);

    EFI_FILE* file;
    status = root->Open(root, &file, path, EFI_FILE_MODE_READ, EFI_FILE_READ_ONLY);
    RETURN_IF_ERROR_STATUS(status);

    VOID* header;
    status = gBS->AllocatePool(EfiLoaderData, sizeof(Elf64_Ehdr), (VOID**)&header);
    RETURN_IF_ERROR_STATUS(status);

    DebugPrint(DEBUG_INFO, "\nLoading Image: %s\n", path);

    *image = (ElfImage){ 0 };

    status = LoadElfHeader(file, header, &image->entry);
    RETURN_IF_ERROR_STATUS(status);

    status = ((Elf64_Ehdr*)header)->e_ident[EI_CLASS] == ELFCLASS64
        ? LoadElf64Image(file, header, &image->start, &image->pageCount)
        : LoadElf32Image(file, header, &image->start, &image->pageCount);
    RETURN_IF_ERROR_STATUS(status);

    image->Execute = (ElfImageExecute*)((UINTN)image->start + (UINTN)image->entry);

    DebugPrint(DEBUG_INFO, "\n%11s: 0x%016x\n%11s: 0x%016x\n%11s: %d\n",
        L"Entry Point", image->entry, L"Image Start", image->start, L"Page Count", image->pageCount);

    FreePool(header);

    return status;
}

EFI_STATUS LoadElfHeader(EFI_FILE* file, VOID* header, VOID** entry) {
    EFI_STATUS status;

    UINTN size = sizeof(Elf64_Ehdr);
    status = file->Read(file, &size, header);
    RETURN_IF_ERROR_STATUS(status);

    Elf32_Ehdr* header32 = header;
    Elf64_Ehdr* header64 = header;

    // Verify magic number
    if (header32->e_ident[EI_MAG0] != ELFMAG0 ||
        header32->e_ident[EI_MAG1] != ELFMAG1 ||
        header32->e_ident[EI_MAG2] != ELFMAG2 ||
        header32->e_ident[EI_MAG3] != ELFMAG3) {
        DEBUG((DEBUG_ERROR, "%a %d Invalid or corrupt header.\n", __FUNCTION__, __LINE__));
        return RETURN_LOAD_ERROR;
    }

    // Only support little-endianness
    if (header32->e_ident[EI_DATA] != ELFDATA2LSB) {
        DEBUG((DEBUG_ERROR, "%a %d Unsupported endianness.\n", __FUNCTION__, __LINE__));
        return RETURN_LOAD_ERROR;
    }

    // Only support executable file type
    if (header32->e_type != ET_EXEC) {
        DEBUG((DEBUG_ERROR, "%a %d Unsupported file type.\n", __FUNCTION__, __LINE__));
        return RETURN_LOAD_ERROR;
    }

    // Only support current ELF version
    if (header32->e_version != EV_CURRENT) {
        DEBUG((DEBUG_ERROR, "%a %d Unsupported file version.\n", __FUNCTION__, __LINE__));
        return RETURN_LOAD_ERROR;
    }

    // Support 32 bit Intel/AMD architectures
    if (header32->e_ident[EI_CLASS] == ELFCLASS32 && header32->e_machine == EM_386) {
        *entry = (VOID*)(UINTN)header32->e_entry;
        return status;
    }

    // Support 64 bit Intel/AMD architectures
    if (header64->e_ident[EI_CLASS] == ELFCLASS64 && header64->e_machine == EM_X86_64) {
        *entry = (VOID*)(UINTN)header64->e_entry;
        return status;
    }

    DEBUG((DEBUG_ERROR, "%a %d Unsupported architecture.\n", __FUNCTION__, __LINE__));

    return RETURN_LOAD_ERROR;
}

EFI_STATUS LoadElf32Image(EFI_FILE* file, Elf32_Ehdr* header, VOID** start, UINTN* pages) {
    DEBUG((DEBUG_ERROR, "%a %d Not implemented.\n", __FUNCTION__, __LINE__));
    return EFI_UNSUPPORTED;
}

EFI_STATUS LoadElf64Image(EFI_FILE* file, Elf64_Ehdr* header, VOID** start, UINTN* pages) {
    EFI_STATUS status;

    // Load program headers
    status = file->SetPosition(file, header->e_phoff);
    RETURN_IF_ERROR_STATUS(status);

    UINTN size = header->e_phnum * header->e_phentsize;

    Elf64_Phdr* programHeaders;
    status = gBS->AllocatePool(EfiLoaderData, size, (VOID**)&programHeaders);
    RETURN_IF_ERROR_STATUS(status);

    status = file->Read(file, &size, programHeaders);
    RETURN_IF_ERROR_STATUS(status);

    // Get total program segment page count
    *pages = 0;
    for (UINTN index = 0; index < header->e_phnum; index++) {
        Elf64_Phdr programHeader = programHeaders[index];

        // Skip any segment that is not loadable
        if (programHeader.p_type != PT_LOAD) {
            continue;
        }

        *pages += (programHeader.p_memsz + 0xFFF) / 0x1000;
    }

    // Allocate enough pages for program code
    VOID* buffer;
    status = gBS->AllocatePages(AllocateAnyPages, EfiLoaderData, *pages, (EFI_PHYSICAL_ADDRESS*)&buffer);
    RETURN_IF_ERROR_STATUS(status);

    VOID* pBuffer = buffer;

    // Load program segments
    for (UINTN index = 0; index < header->e_phnum; index++) {
        Elf64_Phdr programHeader = programHeaders[index];

        // Align buffer pointer
        pBuffer += (programHeader.p_align - (UINTN)pBuffer % programHeader.p_align) % programHeader.p_align;

        // Skip any segment that is not loadable
        if (programHeader.p_type != PT_LOAD) {
            continue;
        }

        DebugPrint(DEBUG_INFO, "Type:%2d, Addr: 0x%016x, Align:%5d, FSize:%6d, MSize:%6d\n",
            programHeader.p_type, programHeader.p_vaddr, programHeader.p_align, programHeader.p_filesz, programHeader.p_memsz);

        status = file->SetPosition(file, programHeader.p_offset);
        RETURN_IF_ERROR_STATUS(status);

        size = programHeader.p_filesz;
        status = file->Read(file, &size, pBuffer);
        RETURN_IF_ERROR_STATUS(status);

        pBuffer += programHeader.p_memsz;
    }

    *start = buffer;

    FreePool(programHeaders);

    return status;
}
