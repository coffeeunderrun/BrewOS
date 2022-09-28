#ifndef BREWOS_COLORS_H
#define BREWOS_COLORS_H

#include <stdint.h>

namespace BrewOS::Graphics
{
    typedef enum class Color
    {
        Transparent = -1,
        Black = 0x000000,
        CornflowerBlue = 0x6495ED,
        White = 0xFFFFFF
    } Color;
}

#endif // BREWOS_COLORS_H
