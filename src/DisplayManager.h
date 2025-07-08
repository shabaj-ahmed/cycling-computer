#ifndef DISPLAY_MANAGER_H
#define DISPLAY_MANAGER_H

#include <vector>
#include <M5Unified.h>
#include "SystemEvent.h"

class DisplayManager {
public:
    DisplayManager();
    void begin(QueueHandle_t queue);

    void updateBPM(float bpm);
    void updateRR(const std::vector<float>& rrIntervals);

private:
    static void displayTask(void* pvParameters);  // FreeRTOS task
    QueueHandle_t eventQueue;

    // Cached values
    float latestBPM;
    std::vector<float> latestRR;

    // Mutex to safely access cached data across tasks
    static SemaphoreHandle_t dataMutex;
};

#endif
