#include <graphics.hpp>

namespace BrewOS::Graphics
{
    static GraphicsMode s_mode;

    static uint8_t *s_frameBuffer;

    static uint64_t s_frameBufferSize;

    static uint32_t s_pixelWidth, s_pitch;

    void Initialize(const GraphicsOutput *graphicsOutput)
    {
        s_mode = graphicsOutput->mode;
        s_frameBuffer = graphicsOutput->frameBuffer;
        s_frameBufferSize = graphicsOutput->frameBufferSize;
        s_pixelWidth = 4;
        s_pitch = s_mode.pixelsPerScanLine * s_pixelWidth;
    }

    void ClearScreen(const uint32_t color)
    {
        for (uint8_t *pFrameBuffer = s_frameBuffer;
             pFrameBuffer < s_frameBuffer + s_frameBufferSize;
             pFrameBuffer += s_pixelWidth)
        {
            *pFrameBuffer = color & 255;
            *(pFrameBuffer + 1) = (color >> 8) & 255;
            *(pFrameBuffer + 2) = (color >> 16) & 255;
        }
    }

    void DrawPoint(const uint32_t x, const uint32_t y, const uint32_t color)
    {
        uint32_t where = x * s_pixelWidth + y * s_pitch;

        s_frameBuffer[where] = color & 255;
        s_frameBuffer[where + 1] = (color >> 8) & 255;
        s_frameBuffer[where + 2] = (color >> 16) & 255;
    }

    uint32_t GetScreenWidth()
    {
        return s_mode.screenWidth;
    }

    uint32_t GetScreenHeight()
    {
        return s_mode.screenHeight;
    }
}