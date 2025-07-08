#include "ButtonManager.h"

ButtonManager::ButtonManager() : eventQueue(nullptr) {}

void ButtonManager::begin(QueueHandle_t queue) {
    eventQueue = queue;

    xTaskCreatePinnedToCore(
        buttonTask,
        "ButtonTask",
        4096,
        this,
        1,
        nullptr,
        1
    );
}

void ButtonManager::buttonTask(void* pvParameters) {
    ButtonManager* self = static_cast<ButtonManager*>(pvParameters);

    bool wasPressedA = false;
    bool wasPressedB = false;
    unsigned long pressStartTime = 0;

    while (true) {
        M5.update();

        bool isPressedA = M5.BtnA.isPressed();
        bool isPressedB = M5.BtnB.isPressed();

        // --------- Button A (Toggle Layout) ----------
        if (isPressedA && !wasPressedA) {
            SystemEvent evt = { EventType::ToggleLayout };
            xQueueSend(self->eventQueue, &evt, portMAX_DELAY);
        }
        wasPressedA = isPressedA;

        // --------- Button B (Short / Long Press) ----------
        if (isPressedB && !wasPressedB) {
            pressStartTime = millis();
        } else if (!isPressedB && wasPressedB) {
            unsigned long pressDuration = millis() - pressStartTime;
            SystemEvent evt = {
                pressDuration > 1500 ? EventType::StopRecording : EventType::StartRecording
            };
            xQueueSend(self->eventQueue, &evt, portMAX_DELAY);
        }
        wasPressedB = isPressedB;

        vTaskDelay(pdMS_TO_TICKS(100));
    }
}
