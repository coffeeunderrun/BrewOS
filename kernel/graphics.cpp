#include <graphics.hpp>

/*** DO NOT USE THESE AFTER INITIALIZATION ***/
extern const BrewOS::Graphics::GraphicsOutput *_graphics_output;
extern const BrewOS::Graphics::Psf *_font;

namespace BrewOS::Graphics
{
    static GraphicsMode s_mode;

    static uint8_t *s_frameBuffer;     // Start of frame buffer
    static uint64_t s_frameBufferSize; // Size of frame buffer in bytes
    static uint32_t s_pixelWidth;      // Width of pixel in bytes
    static uint32_t s_pitch;           // Width of scanline in bytes

    void Initialize()
    {
        s_mode = _graphics_output->mode;
        s_frameBuffer = _graphics_output->frameBuffer;
        s_frameBufferSize = _graphics_output->frameBufferSize;

        // TODO: do not assume the pixel width is always 32 bits
        s_pixelWidth = 4;
        s_pitch = s_mode.pixelsPerScanLine * s_pixelWidth;
    }

    void ClearScreen(const uint32_t color)
    {
        for (uint8_t *where = s_frameBuffer; where < s_frameBuffer + s_frameBufferSize; where += s_pixelWidth)
        {
            *(uint32_t *)where = color;
        }
    }

    void DrawPoint(const uint32_t x, const uint32_t y, const uint32_t color)
    {
        *(uint32_t *)(s_frameBuffer + x * s_pixelWidth + y * s_pitch) = color;
    }

    void DrawChar(const wchar_t c, const uint32_t x, const uint32_t y, const uint32_t color)
    {
        uint8_t *where = s_frameBuffer + x * s_pixelWidth + y * s_pitch;
        uint8_t *glyph = (uint8_t *)_font->glyphBuffer + c * _font->glyphSize;
        uint8_t glyphBytesPerLine = (_font->glyphWidth + 7) / 8;

        for (uint32_t glyphY = 0; glyphY < _font->glyphHeight; glyphY++)
        {
            uint8_t *line = where;
            uint32_t glyphX = 0;

            /*
             * Loop through each line one byte at a time. Anything larger than 8 bits has the
             * potential to load the bitmap in little-endian byte order which is backwards from
             * what we actually want.
             */
            for (uint8_t *glyphByte = glyph; glyphByte < glyph + glyphBytesPerLine; glyphByte++)
            {
                // Start at bit 7 and shift right through all 8 bits
                for (uint8_t glyphBit = 0x80; glyphBit > 0 && glyphX < _font->glyphWidth; glyphBit >>= 1)
                {
                    // Draw pixel when glyph bit is on
                    if (*glyphByte & glyphBit)
                    {
                        *(uint32_t *)line = color;
                    }

                    // Next pixel for framebuffer and glyph
                    line += s_pixelWidth;
                    glyphX++;
                }
            }

            // Next line for framebuffer and glyph
            where += s_pitch;
            glyph += glyphBytesPerLine;
        }
    }

    void DrawText(const wchar_t *s, const uint32_t x, const uint32_t y, const uint32_t color)
    {
        uint32_t cx = x;

        for (wchar_t *c = (wchar_t *)s; *c != 0; c++, cx += _font->glyphWidth)
        {
            DrawChar(*c, cx, y, color);
        }
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