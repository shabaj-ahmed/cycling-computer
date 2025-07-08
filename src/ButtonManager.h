#ifndef BUTTON_MANAGER_H
#define BUTTON_MANAGER_H

#include <M5Unified.h>
#include "SystemEvent.h"

class ButtonManager {
public:
    enum ButtonEvent {
        NONE,
        BUTTON_A_PRESSED,
        BUTTON_B_SHORT_PRESS,
        BUTTON_B_LONG_PRESS
    };

    ButtonManager();
    void begin(QueueHandle_t queue);
    void update();  // Call frequently in a loop or task

    ButtonEvent getLatestEvent();
    void clearEvent();  // Clear after consuming

private:
    unsigned long bPressStart;
    bool bWasHeld;
    ButtonEvent lastEvent;

    static void buttonTask(void* pvParameters);

    QueueHandle_t eventQueue;
};

#endif
