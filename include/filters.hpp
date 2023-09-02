#ifndef _FILTERS_HPP
#define _FILTERS_HPP

#include <Arduino.h>

#define SIGNUM(x) ((x > 0) - (x < 0))

typedef struct {
    int8_t x;
    int8_t y;
} Coords;

Coords filter_coords(Coords coords);
Coords round_up(Coords coords);
Coords scale_up(Coords coords, float multiplier);

/**
 * @brief Check if angle and magnitude is within certain threshold, then snap to specific optimal
 * coordinate
 *
 * @param coords the original coordinates
 * @return Modified coordinates
 */
Coords snap_angles(Coords coords);

#endif