#include "M5Unified.h"
#include "HeartRateSensor.h"
#include "DisplayManager.h"
#include "ButtonManager.h"
#include "SystemEvent.h"
#include "BuzzerManager.h"
#include "RtcManager.h"

HeartRateSensor heartRate;
DisplayManager displayManager;
ButtonManager buttonManager;
BuzzerManager buzzerManager;
RtcManager rtcManager;

QueueHandle_t eventQueue;
QueueHandle_t displayQueue;
QueueHandle_t buzzerQueue;

void dispatcherTask(void* pvParameters) {
    SystemEvent evt;
    while (true) {
        if (xQueueReceive(eventQueue, &evt, portMAX_DELAY) == pdTRUE) {

            if (evt.target == EventTarget::Display || evt.target == EventTarget::All)
                {xQueueSend(displayQueue, &evt, 0);}
            if (evt.target == EventTarget::Buzzer || evt.target == EventTarget::All)
                {xQueueSend(buzzerQueue, &evt, 0);}
        }
    }

    vTaskDelay(pdMS_TO_TICKS(10));  // Prevent task starvation
}

void setup() {
    Serial.begin(115200);
    delay(3000); // Wait for Serial to initialize

    M5.begin();
    eventQueue   = xQueueCreate(20, sizeof(SystemEvent));
    displayQueue = xQueueCreate(30, sizeof(SystemEvent));
    buzzerQueue  = xQueueCreate(10, sizeof(SystemEvent));

    if (!eventQueue) {
        Serial.println("Failed to create event queue!");
        while (true);
    }

    heartRate.begin(eventQueue);
    buttonManager.begin(eventQueue);
    displayManager.begin(displayQueue);
    buzzerManager.begin(buzzerQueue);
    rtcManager.begin(eventQueue);

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

void loop() {}