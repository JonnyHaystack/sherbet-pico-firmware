#ifndef _JOY2KEY_HPP
#define _JOY2KEY_HPP

#include "filters.hpp"

#include <TUKeyboard.hpp>

typedef struct _Directions {
    bool left : 1;
    bool right : 1;
    bool down : 1;
    bool up : 1;
    bool modifier : 1;
} Directions;

void joy2key(Coords coords, TUKeyboard *keyboard);

Directions joy2dirs(Coords coords);

#endif