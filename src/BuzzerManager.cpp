#include "BuzzerManager.h"

BuzzerManager::BuzzerManager() : eventQueue(nullptr) {}

void BuzzerManager::begin(QueueHandle_t queue) {
    this->eventQueue = queue;

    M5.Speaker.setVolume(128);

    BaseType_t result = xTaskCreatePinnedToCore(
        buzzerTask,
        "BuzzerTask",
        2048,
        this,
        1,
        nullptr,
        1
    );
}

void BuzzerManager::buzzerTask(void* pvParameters) {
    BuzzerManager* self = static_cast<BuzzerManager*>(pvParameters);


    while (true) {
        SystemEvent evt;
        while (xQueueReceive(self->eventQueue, &evt, pdMS_TO_TICKS(50)) == pdTRUE) {
            switch (evt.type) {
                case EventType::BuzzerToneStartRecording:
                    M5.Speaker.tone(800, 100);
                    vTaskDelay(pdMS_TO_TICKS(120));
                    M5.Speaker.tone(1200, 100);
                    break;

                case EventType::BuzzerToneStopRecording:
                    M5.Speaker.tone(1200, 100);
                    vTaskDelay(pdMS_TO_TICKS(120));
                    M5.Speaker.tone(800, 100);
                    break;

                case EventType::BuzzerToneToggleLayout:
                    M5.Speaker.tone(1500, 50);
                    vTaskDelay(pdMS_TO_TICKS(60));
                    M5.Speaker.tone(1500, 50);
                    break;

                default:
                    break;
            }
        }

        vTaskDelay(pdMS_TO_TICKS(100));
    }
}