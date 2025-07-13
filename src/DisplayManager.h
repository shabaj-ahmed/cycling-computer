#ifndef DISPLAY_MANAGER_H
#define DISPLAY_MANAGER_H

#include <vector>
#include <M5Unified.h>
#include "SystemEvent.h"

enum class DisplayLayout {
    Default,
    // Add more layouts later
};

class DisplayManager {
public:
    DisplayManager();
    void begin(QueueHandle_t queue);

    void updateBPM(float bpm);
    void updateRR(float rrIntervals);

private:
    static void displayTask(void* pvParameters);  // FreeRTOS task
    QueueHandle_t eventQueue;

    DisplayLayout currentLayout = DisplayLayout::Default;
    bool isRecording = false;
    bool lastRecording = false;
    void drawDefaultLayout();

    // Cached values
    float latestBPM;
    float lastBPM = 0.0f;
    float latestRR;

    // Mutex to safely access cached data across tasks
    static SemaphoreHandle_t dataMutex;
};

#endif
