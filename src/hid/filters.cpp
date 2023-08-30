#include "filters.hpp"

Coords filter_coords(Coords coords) {
    return round_up(coords);
}

uint8_t roundUp(uint8_t coord) {
    if (coord >= 160) {
        return 195;
    }
    if (coord <= 80) {
        return 45;
    }
    return coord;
}

Coords round_up(Coords coords) {
    Coords new_coords;
    new_coords.x = roundUp(coords.x);
    new_coords.y = roundUp(coords.y);
    return new_coords;
}