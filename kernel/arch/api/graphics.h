#pragma once

#include <types.h>

typedef struct GraphicsBitmap {
    uint32_t* data;  // Bitmap data base
    uint32_t width;  // Bitmap width in pixels
    uint32_t height; // Bitmap height in pixels
} GraphicsBitmap;

typedef struct GraphicsColor {
    uint8_t r; // Red
    uint8_t g; // Green
    uint8_t b; // Blue
    uint8_t a; // Alpha
} GraphicsColor;

typedef struct GraphicsPoint {
    uint32_t x; // Y-coordinate
    uint32_t y; // Y-coordinate
} GraphicsPoint;

typedef struct GraphicsRect {
    uint32_t x;      // X-coordinate
    uint32_t y;      // Y-coordinate
    uint32_t width;  // Width in pixels
    uint32_t height; // Height in pixels
} GraphicsRect;

typedef struct GraphicsDeviceContext {
    uint8_t* buffer; // Back buffer base
    uint32_t width;  // Back buffer width in pixels
    uint32_t height; // Back buffer height in pixels

    /// @brief Prepare graphics device for drawing to the back buffer
    /// @param gdc graphics device context
    void (*begin_draw)(struct GraphicsDeviceContext* gdc);

    /// @brief Update front buffer (currently displayed on device) with back buffer
    /// @param gdc graphics device context
    void (*end_draw)(struct GraphicsDeviceContext* gdc);

    /// @brief Clear the back buffer
    /// @param gdc graphics device context
    /// @param color fill color
    void (*clear)(struct GraphicsDeviceContext* gdc, GraphicsColor color);

    /// @brief Draw a single pixel in the back buffer
    /// @param gdc graphics device context
    /// @param point pixel coordinates
    /// @param color pixel color
    void (*draw_point)(struct GraphicsDeviceContext* gdc, GraphicsPoint point, GraphicsColor color);

    /// @brief Draw a rectangle in the back buffer
    /// @param gdc graphics device context
    /// @param rect bounds of rectangle
    /// @param fg foreground (border) color
    /// @param bg background (fill) color
    void (*draw_rect)(struct GraphicsDeviceContext* gdc, GraphicsRect rect, GraphicsColor fg, GraphicsColor bg);

} GraphicsDeviceContext;

GraphicsDeviceContext* create_graphics_device_context();
