#include "filters.hpp"

#include <Arduino.h>
#include <SwitchMatrix.hpp>
#include <TUGamepad.hpp>
#include <TUKeyboard.hpp>

TUGamepad *_gamepad;
TUKeyboard *_keyboard;

const size_t num_rows = 4;
const size_t num_cols = 6;
uint row_pins[num_rows] = { 0, 1, 2, 3 };
uint col_pins[num_cols] = { 4, 5, 6, 7, 8, 9 };

uint8_t matrix[num_rows][num_cols] = {
    {HID_KEY_ESCAPE,        HID_KEY_1, HID_KEY_2, HID_KEY_3, HID_KEY_4, HID_KEY_5},
    { HID_KEY_TAB,          HID_KEY_Q, HID_KEY_W, HID_KEY_E, HID_KEY_R, HID_KEY_T},
    { HID_KEY_CONTROL_LEFT, HID_KEY_A, HID_KEY_S, HID_KEY_D, HID_KEY_F, HID_KEY_G},
    { HID_KEY_SHIFT_LEFT,   HID_KEY_Z, HID_KEY_X, HID_KEY_C, HID_KEY_V, HID_KEY_B},
};

SwitchMatrix<num_rows, num_cols> *_matrix_input = nullptr;

DiodeDirection diode_direction = DiodeDirection::ROW2COL;

// Origin offsets for stick calibration.
int8_t x_offset = 0;
int8_t y_offset = 0;

// Analog stick coordinates, written by core1, read by core0.
volatile Coords _coords;

void setup() {
    _matrix_input =
        new SwitchMatrix<num_rows, num_cols>(row_pins, col_pins, matrix, diode_direction);

    // Read button holds on plugin.
    _matrix_input->Scan([](uint8_t keycode, bool pressed) {
        // Serial.printf("Setting %d to %d\n", keycode, pressed);
        _keyboard->setPressed(keycode, pressed);
    });

    // Hold Escape on plugin to enter BOOTSEL mode.
    if (_keyboard->isPressed(HID_KEY_ESCAPE)) {
        reset_usb_boot(0, 0);
    }

    // Hold 1 on plugin for arrow key joystick mode.
    if (_keyboard->isPressed(HID_KEY_1)) {
        // TODO: Implement arrow key mode
    }

    Serial.begin(115200);

    // TinyUSBDevice.setManufacturerDescriptor("Sony Corp.");
    // TinyUSBDevice.setProductDescriptor("DualShock 4 [CUH-ZCT2x]");
    // TinyUSBDevice.setSerialDescriptor("1.0");
    // TinyUSBDevice.setID(0x054C, 0x09CC);

    TUGamepad::registerDescriptor();
    TUKeyboard::registerDescriptor();

    _gamepad = new TUGamepad();
    _gamepad->begin();
    _keyboard = new TUKeyboard();
    _keyboard->begin();

    while (!USBDevice.mounted()) {
        delay(1);
    }
}

uint8_t counter = 0;

void loop() {
    while (!TUCompositeHID::_usb_hid.ready()) {
        tight_loop_contents();
    }

    /*
     * Keyboard stuff
     */
    _matrix_input->Scan([](uint8_t keycode, bool pressed) {
        _keyboard->setPressed(keycode, pressed);
    });

    _keyboard->sendState();

    while (!TUCompositeHID::_usb_hid.ready()) {
        tight_loop_contents();
    }

    /*
     * Joystick stuff
     */
    _gamepad->leftXAxis(127 - _coords.x);
    _gamepad->leftYAxis(127 - _coords.y);
    _gamepad->sendState();

    // if (counter++ == 200) {
    //     Serial.printf("X Raw: %u, Y Raw: %u X: %d, Y: %d\n", x, y, filtered.x, filtered.y);
    //     counter = 0;
    // }
}

void setup1() {
    analogReadResolution(8);

    // Calibrate stick centre values.
    x_offset = 128 - analogRead(X_PIN);
    y_offset = 128 - analogRead(Y_PIN);

    // _matrix_input =
    //     new SwitchMatrix<num_rows, num_cols>(row_pins, col_pins, matrix, diode_direction);

    // while (_gamepad == nullptr || _keyboard == nullptr) {
    //     tight_loop_contents();
    // }

    // _matrix_input->Scan([](uint8_t keycode, bool pressed) {
    //     _keyboard->setPressed(keycode, pressed);
    // });
    // if (_keyboard->isPressed(HID_KEY_ESCAPE)) {
    //     reset_usb_boot(0, 0);
    // }
}

void loop1() {
    // _matrix_input->Scan([](uint8_t keycode, bool pressed) {
    //     _keyboard->setPressed(keycode, pressed);
    // });

    // Read analog stick values.
    uint8_t x = analogRead(X_PIN) + x_offset;
    uint8_t y = analogRead(Y_PIN) + y_offset;

    // Convert analog values to signed integers and pass them through configured filters.
    Coords filtered = filter_coords({ (int8_t)(x - 128), (int8_t)(y - 128) });
    _coords.x = filtered.x;
    _coords.y = filtered.y;
}