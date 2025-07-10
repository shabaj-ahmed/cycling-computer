#include "ButtonManager.h"

ButtonManager::ButtonManager() : eventQueue(nullptr) {}

void ButtonManager::begin(QueueHandle_t queue) {
    eventQueue = queue;

    xTaskCreatePinnedToCore(
        buttonTask,
        "ButtonTask",
        2048,
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
            SystemEvent layoutEvent = {
                .type = EventType::ToggleLayout,
                .target = EventTarget::Display
            };
            xQueueSend(self->eventQueue, &layoutEvent, portMAX_DELAY);

            SystemEvent buzzerEvent = {
                .type = EventType::BuzzerToneToggleLayout,
                .target = EventTarget::Buzzer
            };
            xQueueSend(self->eventQueue, &buzzerEvent, portMAX_DELAY);
        }
        wasPressedA = isPressedA;

        // --------- Button B (Short / Long Press) ----------
        if (isPressedB && !wasPressedB) {
            pressStartTime = millis();
        } else if (!isPressedB && wasPressedB) {
            unsigned long pressDuration = millis() - pressStartTime;
            if (pressDuration > 1500) {
                SystemEvent stopEvent = {
                    .type = EventType::StopRecording,
                    .target = EventTarget::Display
                };
                xQueueSend(self->eventQueue, &stopEvent, portMAX_DELAY);

                SystemEvent buzzerEvent = {
                    .type = EventType::BuzzerToneStopRecording,
                    .target = EventTarget::Buzzer
                };
                xQueueSend(self->eventQueue, &buzzerEvent, portMAX_DELAY);
            } else {
                SystemEvent startEvent = {
                    .type = EventType::StartRecording,
                    .target = EventTarget::Display
                };
                xQueueSend(self->eventQueue, &startEvent, portMAX_DELAY);

                SystemEvent buzzerEvent = {
                    .type = EventType::BuzzerToneStartRecording,
                    .target = EventTarget::Buzzer
                };
                xQueueSend(self->eventQueue, &buzzerEvent, portMAX_DELAY);
            }
        }
        wasPressedB = isPressedB;

        vTaskDelay(pdMS_TO_TICKS(100));
    }
}
