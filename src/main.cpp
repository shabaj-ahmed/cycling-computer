#include "M5Unified.h"
#include "HeartRateSensor.h"
#include "DisplayManager.h"

HeartRateSensor heartRate;
DisplayManager display;

void setup() {
    Serial.begin(115200);

    heartRate.begin();
    display.begin();
}

void loop() {
}