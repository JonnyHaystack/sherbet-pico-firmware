#include <Adafruit_TinyUSB.h>
#include <Adafruit_USBD_XInput.hpp>
#include <Arduino.h>


Adafruit_USBD_XInput *_xinput;
xinput_report_t _report = {};

void setup() {
    analogReadResolution(8);

    Serial.end();
    _xinput = new Adafruit_USBD_XInput();
    _xinput->begin();
    Serial.begin(115200);

    TinyUSBDevice.setID(0x045e, 0x02A1);

    while (!_xinput->ready()) {
        delay(1);
    }
}

uint8_t counter = 0;

void loop() {
    while (!_xinput->ready()) {
        tight_loop_contents();
    }

    uint8_t x = analogRead(X_PIN);
    uint8_t y = analogRead(Y_PIN);

    // _report.lx = (x - 128) * 100;
    // _report.ly = -(y - 128) * 100;
    // _report.lx = (x - 128) * 65535 / 255;
    // _report.ly = -(y - 128) * 65535 / 255;
    _report.lx = -(map(x, 45, 195, 0, 65535) - 32767);
    _report.ly = map(y, 45, 195, 0, 65535) - 32767;
    // _report.lx = (y - 127) * 100;
    // _report.ly = -(y - 127) * 100;

    _xinput->sendReport(&_report);

    if (counter++ == 200) {
        Serial.printf("X Raw: %u, Y Raw: %u X: %d, Y: %d\n", x, y, _report.lx, _report.ly);
        counter = 0;
    }
}
