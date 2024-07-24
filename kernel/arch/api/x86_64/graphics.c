#include <definitions.h>
#include "graphics.h"

typedef struct GraphicsModePixelBitmask {
    uint32_t red_mask;
    uint32_t green_mask;
    uint32_t blue_mask;
    uint32_t reserved_mask;
} GraphicsModePixelBitmask;

typedef enum GraphicsModePixelFormat {
    PIXEL_FORMAT_RED_GREEN_BLUE_RESERVED,
    PIXEL_FORMAT_BLUE_GREEN_RED_RESERVED,
    PIXEL_FORMAT_BIT_MASK,
    PIXEL_FORMAT_BLT_ONLY,
    PIXEL_FORMAT_MAX,
} GraphicsModePixelFormat;

typedef struct GraphicsMode {
    uint32_t version;
    uint32_t screen_width;
    uint32_t screen_height;
    GraphicsModePixelFormat pixel_format;
    GraphicsModePixelBitmask pixel_bitmask;
    uint32_t pixels_per_scan_line;
} GraphicsMode;

typedef struct GraphicsOutput {
    uint8_t* frame_buffer_base;
    uint64_t frame_buffer_size;
    GraphicsMode mode;
} GraphicsOutput;

#define BUFFER_SIZE (640 * 480 * 4)

static GraphicsOutput graphics;
static uint8_t back_buffer[BUFFER_SIZE];

static GraphicsDeviceContext gdc;

static void begin_draw(GraphicsDeviceContext*);
static void end_draw(GraphicsDeviceContext*);
static void clear(GraphicsDeviceContext*, GraphicsColor);
static void draw_point(GraphicsDeviceContext*, GraphicsPoint, GraphicsColor);
static void draw_rect(GraphicsDeviceContext*, GraphicsRect, GraphicsColor, GraphicsColor);

static GraphicsColor blend(GraphicsColor, GraphicsColor);

BREW_EXPORT BREW_API
void initialize_graphics(GraphicsOutput* graphics_output) {
    graphics = *(GraphicsOutput*)graphics_output;
}

GraphicsDeviceContext* create_graphics_device_context() {
    gdc = (GraphicsDeviceContext){
        .buffer = back_buffer,
        .width = graphics.mode.screen_width,
        .height = graphics.mode.screen_height,
        .begin_draw = begin_draw,
        .end_draw = end_draw,
        .clear = clear,
        .draw_point = draw_point,
        .draw_rect = draw_rect,
    };

    return &gdc;
}

static void begin_draw(GraphicsDeviceContext* gdc) {
}

// A possibility for swapping buffers could be to remap the frame buffer in the page table
static void end_draw(GraphicsDeviceContext* gdc) {
    uint32_t* fptr = (uint32_t*)graphics.frame_buffer_base;
    uint32_t* fend = (uint32_t*)graphics.frame_buffer_base + graphics.frame_buffer_size / 4;
    uint32_t* bptr = (uint32_t*)gdc->buffer;
    uint32_t* bend = (uint32_t*)gdc->buffer + graphics.frame_buffer_size / 4;

    for (; fptr < fend && bptr < bend; fptr++, bptr++) {
        *fptr = *bptr;
    }
}

static void clear(GraphicsDeviceContext* gdc, GraphicsColor color) {
    switch (graphics.mode.pixel_format) {
    case PIXEL_FORMAT_RED_GREEN_BLUE_RESERVED:
        for (uint64_t i = 0; i < BUFFER_SIZE; i += 4) {
            gdc->buffer[i + 0] = color.r;
            gdc->buffer[i + 1] = color.g;
            gdc->buffer[i + 2] = color.b;
        }
        break;

    case PIXEL_FORMAT_BLUE_GREEN_RED_RESERVED:
        for (uint64_t i = 0; i < BUFFER_SIZE; i += 4) {
            gdc->buffer[i + 0] = color.b;
            gdc->buffer[i + 1] = color.g;
            gdc->buffer[i + 2] = color.r;
        }
        break;

    default:
        break;
    }
}

static void draw_point(GraphicsDeviceContext* gdc, GraphicsPoint point, GraphicsColor color) {
    uint64_t pos;
    GraphicsColor bg, blended;

    switch (graphics.mode.pixel_format) {
    case PIXEL_FORMAT_RED_GREEN_BLUE_RESERVED:
        pos = (point.x * 4) + (point.y * 4 * graphics.mode.pixels_per_scan_line);

        bg = (GraphicsColor){ .r = gdc->buffer[pos + 0], .g = gdc->buffer[pos + 1], .b = gdc->buffer[pos + 2] };
        blended = blend(color, bg);

        gdc->buffer[pos + 0] = blended.r;
        gdc->buffer[pos + 1] = blended.g;
        gdc->buffer[pos + 2] = blended.b;
        break;

    case PIXEL_FORMAT_BLUE_GREEN_RED_RESERVED:
        pos = (point.x * 4) + (point.y * 4 * graphics.mode.pixels_per_scan_line);

        bg = (GraphicsColor){ .r = gdc->buffer[pos + 2], .g = gdc->buffer[pos + 1], .b = gdc->buffer[pos + 0] };
        blended = blend(color, bg);

        gdc->buffer[pos + 0] = blended.b;
        gdc->buffer[pos + 1] = blended.g;
        gdc->buffer[pos + 2] = blended.r;
        break;

    default:
        break;
    }
}

static void draw_rect(GraphicsDeviceContext* gdc, GraphicsRect rect, GraphicsColor fg, GraphicsColor bg) {
}

static GraphicsColor blend(GraphicsColor fg, GraphicsColor bg) {
    uint16_t a = fg.a + 1;
    uint16_t i = 256 - fg.a;

    GraphicsColor color = {
        .r = ((a * fg.r + i * bg.r) >> 8) & 0xFF,
        .g = ((a * fg.g + i * bg.g) >> 8) & 0xFF,
        .b = ((a * fg.b + i * bg.b) >> 8) & 0xFF,
        .a = 0xFF,
    };

    return color;
}