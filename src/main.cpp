#include "M5Unified.h"
#include "HeartRateSensor.h"
#include "DisplayManager.h"
#include "ButtonManager.h"
#include "SystemEvent.h"

HeartRateSensor heartRate;
DisplayManager displayManager;
ButtonManager buttonManager;

QueueHandle_t eventQueue;
QueueHandle_t displayQueue;

void dispatcherTask(void* pvParameters) {
    SystemEvent evt;
    while (true) {
        if (xQueueReceive(eventQueue, &evt, portMAX_DELAY) == pdTRUE) {

            if (evt.target == EventTarget::Display || evt.target == EventTarget::All)
                {xQueueSend(displayQueue, &evt, 0);}

        }
    }
}

void setup() {
    Serial.begin(115200);
    delay(3000); // Wait for Serial to initialize

    M5.begin();
    eventQueue   = xQueueCreate(20, sizeof(SystemEvent));
    displayQueue = xQueueCreate(10, sizeof(SystemEvent));

    if (!eventQueue) {
        Serial.println("Failed to create event queue!");
        while (true);
    }

    heartRate.begin(eventQueue);
    buttonManager.begin(eventQueue);
    displayManager.begin(displayQueue);

    xTaskCreatePinnedToCore(
        dispatcherTask,
        "Dispatcher",
        2048,
        nullptr,
        2,
        nullptr,
        1
    );
}

void loop() {

}