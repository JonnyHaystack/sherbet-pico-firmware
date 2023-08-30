#ifndef _FILTERS_HPP
#define _FILTERS_HPP

#include <Arduino.h>

typedef struct {
    uint8_t x;
    uint8_t y;
} Coords;

Coords filter_coords(Coords coords);
Coords round_up(Coords coords);

#endif