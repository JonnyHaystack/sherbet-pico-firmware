#ifndef _DEBOUNCEDSWITCHMATRIX_HPP
#define _DEBOUNCEDSWITCHMATRIX_HPP

#include "SwitchMatrix.hpp"
#include "debounce.hpp"

template <size_t num_rows, size_t num_cols>
class DebouncedSwitchMatrix : public SwitchMatrix<num_rows, num_cols> {
  public:
    DebouncedSwitchMatrix(
        const uint row_pins[num_rows],
        const uint col_pins[num_cols],
        const uint8_t (&keymap)[num_rows][num_cols],
        const DiodeDirection direction,
        DebounceState (&debounce_state)[num_rows][num_cols]
    )
        : SwitchMatrix<num_rows, num_cols>(row_pins, col_pins, keymap, direction),
          _debounce_state(debounce_state) {}

    void Scan(void (*keypress_cb)(uint8_t keycode, bool pressed)) {
        for (size_t i = 0; i < this->_num_outputs; i++) {
            // Activate the column/row.
            pinMode(this->_output_pins[i], OUTPUT);
            digitalWrite(this->_output_pins[i], 0);

            // Read each cell in the column/row.
            for (size_t j = 0; j < this->_num_inputs; j++) {
                uint8_t keycode = this->_direction == DiodeDirection::ROW2COL ? this->_keymap[j][i]
                                                                              : this->_keymap[i][j];
                if (keycode == HID_KEY_NONE) {
                    continue;
                }
                bool key_pressed = !digitalRead(this->_input_pins[j]);
                // Update debounce state, and only call keypress callback if state has changed.
                bool state_changed = update_debounce_state(_debounce_state[i][j], key_pressed);
                if (state_changed && keypress_cb != nullptr) {
                    keypress_cb(keycode, key_pressed);
                }
            }

            // Deactivate the column/row.
            pinMode(this->_output_pins[i], INPUT_PULLUP);
        }
    }

  private:
    DebounceState (&_debounce_state)[num_rows][num_cols];
};

#endif