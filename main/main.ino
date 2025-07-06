#include "HeartRateSensor.h"

HeartRateSensor heartRate;

void setup() {
    Serial.begin(115200);
    heartRate.begin();
    xTaskCreatePinnedToCore(heartRateTask, "HeartRate", 4096, NULL, 1, NULL, 1);
}

void heartRateTask(void* pvParameters) {
    while (true) {
        heartRate.scanAndConnect();
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void loop() {
}