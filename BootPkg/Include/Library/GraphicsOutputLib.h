#pragma once

#include <Uefi.h>
#include <Protocol/GraphicsOutput.h>

typedef struct GraphicsOutput {
    VOID* frameBuffer;
    UINTN frameBufferSize;
    EFI_GRAPHICS_OUTPUT_MODE_INFORMATION modeInfo;
} GraphicsOutput;

/// @brief
/// @param graphicsOutput
/// @param handle
/// @param desiredWidth
/// @param desiredHeight
/// @return
EFI_STATUS GetGraphicsOutput(GraphicsOutput* graphicsOutput, EFI_HANDLE handle, UINTN desiredWidth, UINTN desiredHeight);
