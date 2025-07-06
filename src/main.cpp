#include <Arduino.h>
#include "HeartRateSensor.h"

HeartRateSensor heartRate;

void heartRateTask(void* pvParameters) {
    while (true) {
        heartRate.scanAndConnect();
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void setup() {
    Serial.begin(115200);
    heartRate.begin();
    delay(1000);
    Serial.println("Setup started!");
    xTaskCreatePinnedToCore(heartRateTask, "HeartRate", 4096, NULL, 1, NULL, 1);
}
void loop() {
}