#ifndef BREWOS_COLORS_H
#define BREWOS_COLORS_H

#include <stdint.h>

namespace BrewOS::Graphics
{
    typedef enum class Color
    {
        Transparent = -1,
        Black = 0x000000,
        Blue = 0x0000FF,
        CornflowerBlue = 0x6495ED,
        Green = 0x00FF00,
        Red = 0xFF0000,
        White = 0xFFFFFF
    } Color;
}

#endif // BREWOS_COLORS_H
