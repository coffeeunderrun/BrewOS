#ifndef BREWOS_GRAPHICS_H
#define BREWOS_GRAPHICS_H

#include <stdint.h>

namespace BrewOS::Graphics
{
    typedef struct GraphicsModePixelBitmask
    {
        uint32_t redMask;
        uint32_t greenMask;
        uint32_t blueMask;
        uint32_t reservedMask;
    } GraphicsModePixelBitmask;

    typedef enum class GraphicsModePixelFormat
    {
        PixelRedGreenBlueReserved8BitPerColor,
        PixelBlueGreenRedReserved8BitPerColor,
        PixelBitMask,
        PixelBltOnly,
        PixelFormatMax
    } GraphicsModePixelFormat;

    typedef struct GraphicsMode
    {
        uint32_t version;
        uint32_t screenWidth;
        uint32_t screenHeight;
        GraphicsModePixelFormat pixelFormat;
        GraphicsModePixelBitmask pixelBitmask;
        uint32_t pixelsPerScanLine;
    } GraphicsMode;

    typedef struct GraphicsOutput
    {
        uint8_t *frameBuffer;
        uint64_t frameBufferSize;
        GraphicsMode mode;
    } GraphicsOutput;

    typedef struct Psf
    {
        void *glyphBuffer;
        uint32_t glyphCount;
        uint32_t glyphSize;
        uint32_t glyphHeight;
        uint32_t glyphWidth;
    } Psf;

    void Initialize();

    void ClearScreen(const uint32_t color);

    void DrawPoint(const uint32_t x, const uint32_t y, const uint32_t color);

    void DrawChar(const wchar_t c, const uint32_t x, const uint32_t y, const uint32_t color);

    void DrawText(const wchar_t *s, const uint32_t x, const uint32_t y, const uint32_t color);

    uint32_t GetScreenWidth();

    uint32_t GetScreenHeight();
}

#endif // BREWOS_GRAPHICS_H
