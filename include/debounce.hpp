#ifndef _DEBOUNCE_HPP
#define _DEBOUNCE_HPP

#include <pico/stdlib.h>

typedef struct _DebounceState {
    absolute_time_t locked_until = { ._private_us_since_boot = 0 };
    bool pressed = false;
} DebounceState;

const uint64_t DEBOUNCE_PERIOD_MS = 5;

inline bool update_debounce_state(DebounceState &debounce_state, bool current_reading) {
    // If currently locked out, do nothing.
    if (!time_reached(debounce_state.locked_until)) {
        return false;
    }

    // If latest reading differs from the last known state, update the value and timeout.
    if (current_reading != debounce_state.pressed) {
        debounce_state.pressed = current_reading;
        debounce_state.locked_until = make_timeout_time_ms(DEBOUNCE_PERIOD_MS);
        return true;
    }

    return false;
}

#endif