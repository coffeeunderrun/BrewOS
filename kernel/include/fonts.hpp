#ifndef BREWOS_FONTS_H
#define BREWOS_FONTS_H

#include <stdint.h>

namespace BrewOS::Graphics
{
    typedef struct PsfFont
    {
        void *glyphBuffer;
        uint32_t glyphCount;
        uint32_t glyphSize;
        uint32_t glyphHeight;
        uint32_t glyphWidth;
    } PsfFont;
}

#endif // BREWOS_FONTS_H
