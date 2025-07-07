#include "DisplayManager.h"

SemaphoreHandle_t DisplayManager::dataMutex = nullptr;

DisplayManager::DisplayManager() : latestBPM(0.0f) {
    latestRR.reserve(4);  // Expecting max ~2 R-R values typically
}

void DisplayManager::begin() {
    M5.begin();
    M5.Display.setTextColor(TFT_WHITE);
    M5.Display.setTextSize(2);
    M5.Display.setRotation(1);
    M5.Display.clear();
    M5.Display.println("Display Ready");

    // Ensure the mutex is created before any task uses it
    if (!dataMutex) {
        dataMutex = xSemaphoreCreateMutex();
        if (dataMutex == nullptr) {
            M5.Display.println("Mutex creation failed");
            while (true);  // Halt the system if mutex creation fails
        }
    }

    // Now it's safe to start the task
    xTaskCreatePinnedToCore(
        displayTask,
        "DisplayTask",
        4096,
        this,
        1,
        nullptr,
        1
    );
}

void DisplayManager::updateBPM(float bpm) {
    if (xSemaphoreTake(dataMutex, pdMS_TO_TICKS(10)) == pdTRUE) {
        latestBPM = bpm;
        xSemaphoreGive(dataMutex);
    }
}

void DisplayManager::updateRR(const std::vector<float>& rrIntervals) {
    if (xSemaphoreTake(dataMutex, pdMS_TO_TICKS(10)) == pdTRUE) {
        latestRR = rrIntervals;
        xSemaphoreGive(dataMutex);
    }
}

void DisplayManager::displayTask(void* pvParameters) {
    DisplayManager* self = static_cast<DisplayManager*>(pvParameters);

    while (true) {
        float bpmCopy = 0.0f;
        std::vector<float> rrCopy;

        if (xSemaphoreTake(dataMutex, pdMS_TO_TICKS(50)) == pdTRUE) {
            bpmCopy = self->latestBPM;
            rrCopy = self->latestRR;
            xSemaphoreGive(dataMutex);
        }

        M5.Display.clear();
        M5.Display.setCursor(0, 0);
        M5.Display.printf("BPM: %.1f\n", bpmCopy);
        M5.Display.print("R-R (ms):\n");
        for (float val : rrCopy) {
            M5.Display.printf("%.1f ", val);
        }

        vTaskDelay(pdMS_TO_TICKS(1000));  // Refresh every 1 second
    }
}
