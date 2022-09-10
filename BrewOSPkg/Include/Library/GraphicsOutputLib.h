#ifndef GRAPHICS_OUTPUT_LIB_H
#define GRAPHICS_OUTPUT_LIB_H

#include <Uefi.h>
#include <Protocol/GraphicsOutput.h>

typedef struct GraphicsOutput
{
    VOID *frameBuffer;
    UINTN frameBufferSize;
    EFI_GRAPHICS_OUTPUT_MODE_INFORMATION modeInfo;
} GraphicsOutput;

EFI_STATUS GetGraphicsOutput(GraphicsOutput *graphicsOutput, EFI_HANDLE handle, UINTN desiredWidth, UINTN desiredHeight);

#endif // GRAPHICS_OUTPUT_LIB_H
