#include "screen.h"

static Screen screen;

static void draw_char(Screen*, GraphicsPoint, GraphicsColor, GraphicsColor, Font*, char);
static void draw_text(Screen*, GraphicsPoint, GraphicsColor, GraphicsColor, Font*, char*);

Screen* create_screen(GraphicsDeviceContext* gdc) {
    screen = (Screen){
        .gdc = gdc,
        .draw_text = draw_text,
    };

    return &screen;
}

static void draw_char(Screen* screen, GraphicsPoint point, GraphicsColor fg, GraphicsColor bg, Font* font, char ch) {
    uint8_t* glyph = (uint8_t*)font->glyphs + ch * font->glyph_size;
    uint8_t glyph_bytes_per_line = (font->glyph_width + 7) / 8;

    for (uint32_t glyph_x = 0, glyph_y = 0; glyph_y < font->glyph_height; glyph_x = 0, glyph_y++) {
        // Loop through each line one byte at a time. Anything larger than 8 bits has the potential to
        // load the bitmap in little-endian byte order which is backwards from what we actually want.
        for (uint8_t* glyph_byte = glyph; glyph_byte < glyph + glyph_bytes_per_line; glyph_byte++) {
            // Start at bit 7 and shift right through all 8 bits
            for (uint8_t glyph_bit = 0x80; glyph_bit > 0 && glyph_x < font->glyph_width; glyph_bit >>= 1) {
                // Use foreground color when glyph bit is on, otherwise use background color
                screen->gdc->draw_point(screen->gdc, (GraphicsPoint) { point.x + glyph_x, point.y + glyph_y }, * glyph_byte& glyph_bit ? fg : bg);
                glyph_x++;
            }
        }

        glyph += glyph_bytes_per_line;
    }
}

static void draw_text(Screen* screen, GraphicsPoint point, GraphicsColor fg, GraphicsColor bg, Font* font, char* text) {
    for (char* ch = text; *ch != 0; point.x += font->glyph_width, ch++) {
        draw_char(screen, point, fg, bg, font, *ch);
    }
}
