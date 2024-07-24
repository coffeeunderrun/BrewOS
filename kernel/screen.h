#pragma once

#include <graphics.h>
#include "font.h"

typedef struct Screen {
    GraphicsDeviceContext* gdc; // Graphics device context

    /// @brief Draw text in the graphics device context back buffer
    /// @param screen screen
    /// @param point pixel coordinates
    /// @param fg foreground color
    /// @param bg background color
    /// @param font font
    /// @param text text
    void(*draw_text)(struct Screen* screen, GraphicsPoint point, GraphicsColor fg, GraphicsColor bg, Font* font, char* text);
} Screen;

Screen* create_screen(GraphicsDeviceContext* context);
