#include <Common.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/GraphicsOutputLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiBootServicesTableLib.h>

EFI_STATUS GetGraphicsOutput(GraphicsOutput *graphicsOutput, EFI_HANDLE handle, UINTN desiredWidth, UINTN desiredHeight)
{
    ASSERT(graphicsOutput != NULL);

    EFI_STATUS status;

    UINTN handleCount = 0;
    EFI_HANDLE *handleBuffer;
    status = gBS->LocateHandleBuffer(ByProtocol, &gEfiGraphicsOutputProtocolGuid, NULL, &handleCount, &handleBuffer);
    RETURN_IF_ERROR_STATUS(status);

    EFI_GRAPHICS_OUTPUT_PROTOCOL *gop;
    status = gBS->OpenProtocol(*handleBuffer, &gEfiGraphicsOutputProtocolGuid, (void **)&gop, handle, NULL, EFI_OPEN_PROTOCOL_BY_HANDLE_PROTOCOL);
    RETURN_IF_ERROR_STATUS(status);

    FreePool(handleBuffer);

    UINTN infoSize, mode;
    EFI_GRAPHICS_OUTPUT_MODE_INFORMATION *info;
    for (mode = gop->Mode->MaxMode - 1; mode > 0; mode--)
    {
        status = gop->QueryMode(gop, mode, &infoSize, &info);
        RETURN_IF_ERROR_STATUS(status);

        if (info->HorizontalResolution <= desiredWidth && info->VerticalResolution <= desiredHeight)
        {
            break;
        }
    }

    status = gop->SetMode(gop, mode);
    RETURN_IF_ERROR_STATUS(status);

    DebugPrint(DEBUG_INFO, "Graphics Mode:%2d, Width:%4d, Height:%4d, Format:%2d\n",
               gop->Mode->Mode,
               gop->Mode->Info->HorizontalResolution,
               gop->Mode->Info->VerticalResolution,
               gop->Mode->Info->PixelFormat);

    graphicsOutput->frameBuffer = (VOID *)gop->Mode->FrameBufferBase;
    graphicsOutput->frameBufferSize = gop->Mode->FrameBufferSize;

    CopyMem(&graphicsOutput->modeInfo, gop->Mode->Info, sizeof(EFI_GRAPHICS_OUTPUT_MODE_INFORMATION));

    return status;
}
