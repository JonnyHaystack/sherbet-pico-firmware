#include "joy2key.hpp"

#include "filters.hpp"

#include <TUKeyboard.hpp>

#define DEADZONE 25
#define MODIFIER_THRESHOLD 50

Directions joy2dirs(Coords coords) {
    Directions result = Directions{
        .left = false,
        .right = false,
        .down = false,
        .up = false,
        .modifier = false,
    };
    int8_t x_sign = SIGNUM(coords.x);
    int8_t y_sign = SIGNUM(coords.y);
    int absolute_x = abs(coords.x);
    int absolute_y = abs(coords.y);

    // Calculate angle and magnitude.
    int angle = round(atan2(absolute_x, absolute_y) * (180.0 / M_PI));
    int magnitude = sqrt(sq(absolute_x) + sq(absolute_y));

    if (magnitude < DEADZONE) {
        return result;
    }
    if (magnitude < MODIFIER_THRESHOLD) {
        result.modifier = true;
    }
    if (70 <= angle && angle <= 90) {
        if (x_sign < 0) {
            result.left = true;
        } else if (x_sign > 0) {
            result.right = true;
        }
    } else if (0 <= angle && angle <= 20) {
        if (y_sign < 0) {
            result.down = true;
        } else if (y_sign > 0) {
            result.up = true;
        }
    } else {
        if (x_sign < 0) {
            result.left = true;
        } else if (x_sign > 0) {
            result.right = true;
        }
        if (y_sign < 0) {
            result.down = true;
        } else if (y_sign > 0) {
            result.up = true;
        }
    }
    Serial.printf(
        "magnitude: %d, angle: %d, coords.x: %d, coords.y: %d, left: %d, right: %d, down: %d, up: %d\n",
        magnitude,
        angle,
        coords.x,
        coords.y,
        result.left,
        result.right,
        result.down,
        result.up
    );
    return result;
}

void joy2key(Coords coords, TUKeyboard *keyboard) {
    Directions directions = joy2dirs(coords);
    keyboard->setPressed(HID_KEY_SHIFT_RIGHT, directions.modifier);
    keyboard->setPressed(HID_KEY_ARROW_LEFT, directions.left);
    keyboard->setPressed(HID_KEY_ARROW_RIGHT, directions.right);
    keyboard->setPressed(HID_KEY_ARROW_DOWN, directions.down);
    keyboard->setPressed(HID_KEY_ARROW_UP, directions.up);
}