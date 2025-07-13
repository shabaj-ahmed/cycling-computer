#include "DisplayManager.h"

SemaphoreHandle_t DisplayManager::dataMutex = nullptr;

DisplayManager::DisplayManager() {}

void DisplayManager::begin(QueueHandle_t queue) {
    this->eventQueue = queue;

    M5.Display.setTextColor(TFT_WHITE);
    M5.Display.setTextSize(2);

    M5.Display.setRotation(0);
    
    M5.Display.clear();

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
        3072,
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

void DisplayManager::updateRR(float rrIntervals) {
    if (xSemaphoreTake(dataMutex, pdMS_TO_TICKS(10)) == pdTRUE) {
        latestRR = rrIntervals;
        xSemaphoreGive(dataMutex);
    }
}

void DisplayManager::displayTask(void* pvParameters) {
    DisplayManager* self = static_cast<DisplayManager*>(pvParameters);


    unsigned long lastDisplayUpdate = 0;
    const unsigned long displayInterval = 1000;  // 1s refresh

    while (true) {
        SystemEvent evt;
        while (xQueueReceive(self->eventQueue, &evt, 0) == pdTRUE) {
            switch (evt.type) {
                case EventType::StartRecording:
                    self->isRecording = true;
                    break;
                case EventType::StopRecording:
                    self->isRecording = false;
                    break;
                case EventType::ToggleLayout:
                    self->currentLayout = static_cast<DisplayLayout>(
                        (static_cast<int>(self->currentLayout) + 1) % 1);
                    break;
                case EventType::HeartRateUpdate:
                    if (evt.floatValue >= 0) {
                        self->updateBPM(evt.floatValue);
                    }
                    break;
                case EventType::RRIntervalUpdate:
                    if (evt.floatValue >= 0) {
                        self->updateRR(evt.floatValue);
                    }
                    break;
            }
        }

        // 2. Refresh display every second
        if (millis() - lastDisplayUpdate >= displayInterval) {
            switch (self->currentLayout) {
                case DisplayLayout::Default:
                    self->drawDefaultLayout();
                    break;
                // case DisplayLayout::Another:
                //     self->drawAnotherLayout(...);
                //     break;
            }
            lastDisplayUpdate = millis();
        }

        vTaskDelay(pdMS_TO_TICKS(10));  // Refresh every second
    }
}

void DisplayManager::drawDefaultLayout() {
    M5.Display.setTextColor(TFT_WHITE);

    // 1. Time & recording dot (Row 1)
    M5.Display.setTextSize(3);
    M5.Display.setCursor(10, 10);  // Top-left
    M5.Display.printf("01:26");
    if (isRecording != lastRecording) {
        if (isRecording) {
            M5.Display.fillCircle(115, 20, 8, TFT_RED);
        } else {
            M5.Display.fillCircle(115, 20, 8, TFT_BLACK);  // Erase it
        }
        lastRecording = isRecording;
    }

    // 2. Heart Rate Bar (Row 2)
    if (latestBPM != lastBPM) {
        Serial.printf("Updating BPM: %.1f\n", latestBPM);
        int barX = 10;
        int barY = 50;
        int barW = 115;
        int barH = 30;

        // Erase previous bar
        M5.Display.fillRect(barX + 1, barY + 1, barW - 2, barH - 2, TFT_BLACK);

        // Draw new bar
        float barWidth = std::min(barW, static_cast<int>(barW * latestBPM / 200.0f));
        uint16_t color = TFT_WHITE;
        if (latestBPM < 90) color = TFT_ORANGE;
        else if (latestBPM < 140) color = TFT_GREEN;
        else color = TFT_RED;

        M5.Display.fillRect(barX + 1, barY + 1, barWidth - 2, barH - 2, color);

        lastBPM = latestBPM;
    }

    // 3. Cadence (Row 3)
    M5.Display.setTextSize(6);  // Larger text for cadence
    M5.Display.setCursor(10, 95);  // Y position shifted down
    M5.Display.printf("85");
    M5.Display.setTextSize(2);  // Larger text for cadence
    M5.Display.printf("rpm");

    // 4. Speed (Row 4)
    M5.Display.setTextSize(3);  // Medium text for speed
    M5.Display.setCursor(10, 185);
    M5.Display.printf("18.2");
    M5.Display.setTextSize(2);
    M5.Display.printf("mph");
}
