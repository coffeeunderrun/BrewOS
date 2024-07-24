#include "font.h"

// PSF Version 1
#define PSF1_MAGIC 0x0436

#define PSF1_MODE512    0x01
#define PSF1_MODEHASTAB 0x02
#define PSF1_MODEHASSEQ 0x04
#define PSF1_MAXMODE    0x05

#define PSF1_SEPARATOR 0xFFFF
#define PSF1_START_SEQ 0xFFFE

// PSF Version 2
#define PSF2_MAGIC 0x864AB572

#define PSF2_HAS_UNICODE_TABLE 0x01

#define PSF2_MAXVERSION 0
#define PSF2_SEPARATOR  0xFF
#define PSF2_START_SEQ  0xFE

typedef struct PSF1Header {
    uint16_t magic;
    uint8_t mode;
    uint8_t glyph_size;
} PSF1Header;

typedef struct PSF2Header {
    uint32_t magic;
    uint32_t version;
    uint32_t header_size;
    uint32_t flags;
    uint32_t glyph_count;
    uint32_t glyph_size;
    uint32_t glyph_height;
    uint32_t glyph_width;
} PSF2Header;

extern const void* kernel_font_start;
extern const void* kernel_font_end;

static Font s_font;

static Font* get_font_from_psf1_header(PSF1Header*);
static Font* get_font_from_psf2_header(PSF2Header*);

Font* get_kernel_font() {
    return &s_font;
}

Font* load_kernel_font() {
    Font* font = NULL;

    font = get_font_from_psf1_header((PSF1Header*)&kernel_font_start);
    if (font != NULL) {
        return font;
    }

    font = get_font_from_psf2_header((PSF2Header*)&kernel_font_start);
    if (font != NULL) {
        return font;
    }

    return NULL;
}

static Font* get_font_from_psf1_header(PSF1Header* header) {
    if (header->magic != PSF1_MAGIC) {
        return NULL;
    }

    Font* font = &s_font;

    font->glyphs = (uint8_t*)header + sizeof(PSF1Header);
    font->glyph_count = header->mode & PSF1_MODE512 ? 512 : 256;
    font->glyph_size = header->glyph_size;
    font->glyph_height = header->glyph_size;
    font->glyph_width = 8;

    return font;
}

static Font* get_font_from_psf2_header(PSF2Header* header) {
    if (header->magic != PSF2_MAGIC) {
        return NULL;
    }

    Font* font = &s_font;

    font->glyphs = (uint8_t*)header + header->header_size;
    font->glyph_count = header->glyph_count;
    font->glyph_size = header->glyph_size;
    font->glyph_height = header->glyph_height;
    font->glyph_width = header->glyph_width;

    return font;
}
