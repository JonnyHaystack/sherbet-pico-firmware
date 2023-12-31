#include "filters.hpp"

Coords filter_coords(Coords coords) {
    return snap_angles(scale_up(coords, 1.8));
}

int8_t scale_up_single(int8_t coord, float multiplier) {
    int8_t sign = SIGNUM(coord);
    int absolute = abs(coord);
    return min(127, (int)(absolute * multiplier)) * sign;
}

int8_t round_up_single(int8_t coord) {
    int8_t sign = SIGNUM(coord);
    int absolute = abs(coord);
    if (absolute > 40) {
        return 75 * sign;
    }
    return coord;
}

Coords round_up(Coords coords) {
    return Coords{ round_up_single(coords.x), round_up_single(coords.y) };
}

Coords scale_up(Coords coords, float multiplier) {
    return Coords{ scale_up_single(coords.x, multiplier), scale_up_single(coords.y, multiplier) };
}

Coords snap_angles(Coords coords) {
    int8_t x_sign = SIGNUM(coords.x);
    int8_t y_sign = SIGNUM(coords.y);
    int absolute_x = abs(coords.x);
    int absolute_y = abs(coords.y);

    // Calculate angle and magnitude.
    int angle = round(atan2(absolute_x, absolute_y) * (180.0 / M_PI));
    int magnitude = sqrt(sq(absolute_x) + sq(absolute_y));
    if (magnitude < 90) {
        return coords;
    }
    if (y_sign >= 0) {
        if (30 <= angle && angle <= 65) {
            // 60 degree forward diagonal.
            return Coords{ (int8_t)(109 * x_sign), 64 };
        }
        if (65 <= angle && angle <= 90) {
            // 76 degree strafe angle.
            return Coords{ (int8_t)(123 * x_sign), 33 };
        }
    }
    if (y_sign <= 0) {
        if (80 <= angle && angle <= 90) {
            // 76 degree strafe angle.
            return Coords{ (int8_t)(123 * x_sign), 33 };
        }
    }
    return coords;
}