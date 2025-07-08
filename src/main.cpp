#include "M5Unified.h"
#include "HeartRateSensor.h"
#include "DisplayManager.h"
#include "ButtonManager.h"
#include "SystemEvent.h"

HeartRateSensor heartRate;
DisplayManager displayManager;
ButtonManager buttonManager;

QueueHandle_t eventQueue;

void setup() {
    Serial.begin(115200);

    QueueHandle_t eventQueue;

    eventQueue = xQueueCreate(10, sizeof(SystemEvent));
    if (!eventQueue) {
        Serial.println("Failed to create event queue!");
        while (true);
    }

    heartRate.begin(eventQueue);
    buttonManager.begin(eventQueue);
    displayManager.begin(eventQueue);
}

void loop() {
}