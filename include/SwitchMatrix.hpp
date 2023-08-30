#ifndef _SWITCHMATRIX_HPP
#define _SWITCHMATRIX_HPP

#include <Arduino.h>
#include <Adafruit_TinyUSB.h>

#define NA HID_KEY_NONE

enum class DiodeDirection {
    ROW2COL,
    COL2ROW,
};

// TUKeyboard *_keyboard;

template <size_t num_rows, size_t num_cols> class SwitchMatrix {
  public:
    SwitchMatrix(
        uint row_pins[num_rows],
        uint col_pins[num_cols],
        uint8_t (&matrix)[num_rows][num_cols],
        DiodeDirection direction
    )
        : _matrix(matrix) {
        _direction = direction;

        if (_direction == DiodeDirection::ROW2COL) {
            _num_outputs = num_cols;
            _num_inputs = num_rows;
            _output_pins = col_pins;
            _input_pins = row_pins;
        } else {
            _num_outputs = num_rows;
            _num_inputs = num_cols;
            _output_pins = row_pins;
            _input_pins = col_pins;
        }

        // Initialize output pins.
        for (size_t i = 0; i < _num_outputs; i++) {
            // gpio::init_pin(_output_pins[i], gpio::GpioMode::GPIO_INPUT_PULLUP);
            // gpio_init(_output_pins[i]);
            // gpio_set_dir(_output_pins[i], GPIO_IN);
            // gpio_pull_up(_output_pins[i]);
            pinMode(_output_pins[i], INPUT_PULLUP);
        }

        // Initialize input pins.
        for (size_t i = 0; i < _num_inputs; i++) {
            // gpio::init_pin(_input_pins[i], gpio::GpioMode::GPIO_INPUT_PULLUP);
            // gpio_init(_input_pins[i]);
            // gpio_set_dir(_input_pins[i], GPIO_IN);
            // gpio_pull_up(_input_pins[i]);
            pinMode(_input_pins[i], INPUT_PULLUP);
        }
    }

    ~SwitchMatrix() {
        // Make sure all pins are set back to inputs.
        for (size_t i = 0; i < _num_outputs; i++) {
            // gpio::init_pin(_output_pins[i], gpio::GpioMode::GPIO_INPUT_PULLUP);
            // gpio_init(_output_pins[i]);
            // gpio_set_dir(_output_pins[i], GPIO_IN);
            // gpio_pull_up(_output_pins[i]);
            pinMode(_output_pins[i], INPUT_PULLUP);
        }
    }

    void Scan(void (*keypress_cb)(uint8_t keycode, bool pressed)) {
        for (size_t i = 0; i < _num_outputs; i++) {
            // Activate the column/row.
            // gpio::init_pin(_output_pins[i], gpio::GpioMode::GPIO_OUTPUT);
            // gpio::write_digital(_output_pins[i], 0);
            // gpio_init(_output_pins[i]);
            // gpio_set_dir(_output_pins[i], GPIO_OUT);
            // gpio_put(_output_pins[i], 0);
            pinMode(_output_pins[i], OUTPUT);
            digitalWrite(_output_pins[i], 0);

            // Read each cell in the column/row.
            for (size_t j = 0; j < _num_inputs; j++) {
                uint8_t keycode =
                    _direction == DiodeDirection::ROW2COL ? _matrix[j][i] : _matrix[i][j];
                if (keycode != HID_KEY_NONE) {
                    keypress_cb(keycode, !digitalRead(_input_pins[j]));
                    // keypress_cb(keycode, !gpio_get(_input_pins[j]));
                }
            }

            // Deactivate the column/row.
            // gpio::init_pin(_output_pins[i], gpio::GpioMode::GPIO_INPUT_PULLUP);
            // gpio_init(_output_pins[i]);
            // gpio_set_dir(_output_pins[i], GPIO_IN);
            // gpio_pull_up(_output_pins[i]);
            pinMode(_output_pins[i], INPUT_PULLUP);
        }
    }

  protected:
    size_t _num_outputs;
    size_t _num_inputs;
    uint *_output_pins;
    uint *_input_pins;
    uint8_t (&_matrix)[num_rows][num_cols];
    DiodeDirection _direction;
};

#endif