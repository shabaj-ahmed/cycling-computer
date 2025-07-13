#include "RtcManager.h"

RtcManager::RtcManager() : eventQueue(nullptr) {}

void RtcManager::begin(QueueHandle_t queue) {
    this->eventQueue = queue;

    if (!M5.Rtc.isEnabled()) {
        Serial.println("RTC not found.");
        M5.Display.println("RTC not found.");
        for (;;) {
            vTaskDelay(500);
        }
    }

    BaseType_t result = xTaskCreatePinnedToCore(
        rtcTask,
        "RtcTask",
        1024,
        this,
        1,
        nullptr,
        1
    );
}

void RtcManager::rtcTask(void* pvParameters) {
    RtcManager* self = static_cast<RtcManager*>(pvParameters);

    while (true) {
        auto currentTime = M5.Rtc.getDateTime();

        SystemEvent rtcEvent = {
            .type = EventType::RtcHourUpdate,
            .target = EventTarget::Display,
            .floatValue = static_cast<float>(currentTime.time.hours)
        };
        xQueueSend(self->eventQueue, &rtcEvent, 0);

        SystemEvent rtcMinuteEvent = {
            .type = EventType::RtcMinuteUpdate,
            .target = EventTarget::Display,
            .floatValue = static_cast<float>(currentTime.time.minutes)
        };
        xQueueSend(self->eventQueue, &rtcMinuteEvent, 0);

        vTaskDelay(pdMS_TO_TICKS(1000));  // loop every 1 second
    }
}
