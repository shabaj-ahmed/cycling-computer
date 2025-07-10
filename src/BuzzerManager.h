#ifndef BUZZER_MANAGER_H
#define BUZZER_MANAGER_H

#include <M5Unified.h>
#include "SystemEvent.h"

class BuzzerManager {
public:
    BuzzerManager();
    void begin(QueueHandle_t queue);

private:
    static void buzzerTask(void* pvParameters);

    QueueHandle_t eventQueue;
    void playToneSequence(const std::vector<std::pair<int, int>>& tones);
};

#endif
