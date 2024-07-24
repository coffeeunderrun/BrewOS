#pragma once

#include <types.h>

typedef struct Font {
    uint8_t* glyphs;
    uint32_t glyph_count;
    uint32_t glyph_size;
    uint32_t glyph_height;
    uint32_t glyph_width;
} Font;

Font* get_kernel_font();

Font* load_kernel_font();
