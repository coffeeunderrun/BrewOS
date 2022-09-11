#include <Common.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/PsfFontLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Protocol/LoadedImage.h>
#include <Protocol/SimpleFileSystem.h>

EFI_STATUS LoadPsfHeader(EFI_FILE *file, VOID *header);
EFI_STATUS LoadPsf1Image(EFI_FILE *file, Psf1Header *header, PsfImage *image);
EFI_STATUS LoadPsf2Image(EFI_FILE *file, Psf2Header *header, PsfImage *image);

EFI_STATUS LoadPsfImage(CHAR16 *fileName, EFI_HANDLE handle, PsfImage *image)
{
    ASSERT(image != NULL);

    EFI_STATUS status;

    EFI_LOADED_IMAGE_PROTOCOL *loadedImage;
    status = gBS->HandleProtocol(handle, &gEfiLoadedImageProtocolGuid, (VOID **)&loadedImage);
    RETURN_IF_ERROR_STATUS(status);

    EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *fileSystem;
    status = gBS->HandleProtocol(loadedImage->DeviceHandle, &gEfiSimpleFileSystemProtocolGuid, (VOID **)&fileSystem);
    RETURN_IF_ERROR_STATUS(status);

    EFI_FILE *root;
    status = fileSystem->OpenVolume(fileSystem, &root);
    RETURN_IF_ERROR_STATUS(status);

    EFI_FILE *file;
    status = root->Open(root, &file, fileName, EFI_FILE_MODE_READ, EFI_FILE_READ_ONLY);
    RETURN_IF_ERROR_STATUS(status);

    VOID *header;
    status = gBS->AllocatePool(EfiLoaderData, sizeof(Psf2Header), (VOID **)&header);
    RETURN_IF_ERROR_STATUS(status);

    DebugPrint(DEBUG_INFO, "\nLoading Font: %s\n", fileName);

    status = LoadPsfHeader(file, header);
    RETURN_IF_ERROR_STATUS(status);

    Psf1Header *psf1Header = header;
    if (psf1Header->magic[0] == PSF1_MAGIC0 && psf1Header->magic[1] == PSF1_MAGIC1)
    {
        status = LoadPsf1Image(file, header, image);
    }
    else
    {
        status = LoadPsf2Image(file, header, image);
    }
    RETURN_IF_ERROR_STATUS(status);

    DebugPrint(DEBUG_INFO, "Addr: %16p, Count:%4d, Size:%3d, Width:%3d, Height:%3d\n",
               image->glyphBuffer, image->glyphCount, image->glyphSize, image->glyphWidth, image->glyphHeight);

    FreePool(header);

    return status;
}

EFI_STATUS LoadPsfHeader(EFI_FILE *file, VOID *header)
{
    ASSERT(file != NULL);

    EFI_STATUS status;

    UINTN size = sizeof(Psf2Header);
    status = file->Read(file, &size, header);
    RETURN_IF_ERROR_STATUS(status);

    Psf1Header *psf1Header = header;
    Psf2Header *psf2Header = header;

    // Check for PSF version 1 magic number
    if (psf1Header->magic[0] == PSF1_MAGIC0 && psf1Header->magic[1] == PSF1_MAGIC1)
    {
        return status;
    }

    // Check for PSF version 2 magic number
    if (psf2Header->magic[0] == PSF2_MAGIC0 &&
        psf2Header->magic[1] == PSF2_MAGIC1 &&
        psf2Header->magic[2] == PSF2_MAGIC2 &&
        psf2Header->magic[3] == PSF2_MAGIC3)
    {
        return status;
    }

    return RETURN_LOAD_ERROR;
}

EFI_STATUS LoadPsf1Image(EFI_FILE *file, Psf1Header *header, PsfImage *image)
{
    ASSERT(file != NULL);
    ASSERT(header != NULL);

    EFI_STATUS status;

    BOOLEAN isMode512 = header->mode & PSF1_MODE512;
    UINTN bufferSize = header->glyphSize * (isMode512 ? 512 : 256);

    status = gBS->AllocatePool(EfiLoaderData, bufferSize, (VOID **)&image->glyphBuffer);
    RETURN_IF_ERROR_STATUS(status);

    status = file->SetPosition(file, sizeof(Psf1Header));
    RETURN_IF_ERROR_STATUS(status);

    status = file->Read(file, &bufferSize, image->glyphBuffer);
    RETURN_IF_ERROR_STATUS(status);

    image->glyphCount = isMode512 ? 512 : 256;
    image->glyphSize = header->glyphSize;
    image->glyphHeight = header->glyphSize;
    image->glyphWidth = 8;

    return status;
}

EFI_STATUS LoadPsf2Image(EFI_FILE *file, Psf2Header *header, PsfImage *image)
{
    ASSERT(file != NULL);
    ASSERT(header != NULL);

    EFI_STATUS status;

    UINTN bufferSize = header->glyphCount * header->glyphSize;

    status = gBS->AllocatePool(EfiLoaderData, bufferSize, (VOID **)&image->glyphBuffer);
    RETURN_IF_ERROR_STATUS(status);

    status = file->SetPosition(file, header->headerSize);
    RETURN_IF_ERROR_STATUS(status);

    status = file->Read(file, &bufferSize, image->glyphBuffer);
    RETURN_IF_ERROR_STATUS(status);

    image->glyphCount = header->glyphCount;
    image->glyphSize = header->glyphSize;
    image->glyphHeight = header->glyphHeight;
    image->glyphWidth = header->glyphWidth;

    return status;
}
