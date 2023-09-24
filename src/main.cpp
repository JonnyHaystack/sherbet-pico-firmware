#include "filters.hpp"
#include "joy2key.hpp"

#include <Arduino.h>
#include <SwitchMatrix.hpp>
#include <TUGamepad.hpp>
#include <TUKeyboard.hpp>

typedef enum _JoystickMode {
    JOYSTICK_ANALOG,
    JOYSTICK_ARROW_KEYS,
} JoystickMode;

JoystickMode _joystick_mode = JOYSTICK_ANALOG;

TUGamepad *_gamepad;
TUKeyboard *_keyboard;

const size_t num_rows = 4;
const size_t num_cols = 6;
uint row_pins[num_rows] = { 0, 1, 2, 3 };
uint col_pins[num_cols] = { 4, 5, 6, 7, 8, 9 };

uint8_t keymap[num_rows][num_cols] = {
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
        new SwitchMatrix<num_rows, num_cols>(row_pins, col_pins, keymap, diode_direction);

    _keyboard = new TUKeyboard();
    _keyboard->releaseAll();

    // Read button holds on plugin.
    _matrix_input->Scan([](uint8_t keycode, bool pressed) {
        _keyboard->setPressed(keycode, pressed);
    });

    // Hold Escape on plugin to enter BOOTSEL mode.
    if (_keyboard->isPressed(HID_KEY_ESCAPE)) {
        reset_usb_boot(0, 0);
    }

    // Hold Shift on plugin for arrow key joystick mode.
    if (_keyboard->isPressed(HID_KEY_SHIFT_LEFT)) {
        // TODO: Implement arrow key mode
        _joystick_mode = JOYSTICK_ARROW_KEYS;
    }

    Serial.begin(115200);

    TinyUSBDevice.setManufacturerDescriptor("Sherbet");
    TinyUSBDevice.setProductDescriptor("Gaming Keypad");
    // TinyUSBDevice.setSerialDescriptor("1.0");
    TinyUSBDevice.setID(0x054C, 0x09CC);

    if (_joystick_mode == JOYSTICK_ANALOG) {
        TUGamepad::registerDescriptor();
    }
    TUKeyboard::registerDescriptor();

    if (_joystick_mode == JOYSTICK_ANALOG) {
        _gamepad = new TUGamepad();
        _gamepad->begin();
    }
    _keyboard->begin();

    while (!USBDevice.mounted()) {
        delay(1);
    }
}

uint8_t counter = 0;

void loop() {
    /*
     * Keyboard stuff
     */
    while (!TUCompositeHID::_usb_hid.ready()) {
        tight_loop_contents();
    }
    _matrix_input->Scan([](uint8_t keycode, bool pressed) {
        _keyboard->setPressed(keycode, pressed);
    });
    if (_joystick_mode == JOYSTICK_ARROW_KEYS) {
        joy2key({ (int8_t)-_coords.x, _coords.y }, _keyboard);
    }
    _keyboard->sendState();

    /*
     * Joystick stuff
     */
    if (_joystick_mode == JOYSTICK_ANALOG) {
        while (!_gamepad->ready()) {
            tight_loop_contents();
        }
        _gamepad->leftXAxis(127 - _coords.x);
        _gamepad->leftYAxis(127 - _coords.y);
        _gamepad->sendState();
    }
}

void setup1() {
    analogReadResolution(8);

    // Calibrate stick centre values.
    x_offset = 128 - analogRead(X_PIN);
    y_offset = 128 - analogRead(Y_PIN);
}

void loop1() {
    // Read analog stick values.
    uint8_t x = analogRead(X_PIN) + x_offset;
    uint8_t y = analogRead(Y_PIN) + y_offset;

    if (_joystick_mode == JOYSTICK_ANALOG) {
        // Convert analog values to signed integers and pass them through configured filters.
        Coords filtered = filter_coords({ (int8_t)(x - 128), (int8_t)(y - 128) });
        _coords.x = filtered.x;
        _coords.y = filtered.y;
    } else {
        _coords.x = (int8_t)(x - 128);
        _coords.y = (int8_t)(y - 128);
    }
}