#ifndef RTC_H
#define RTC_H

#include <M5Unified.h>
#include "SystemEvent.h"

class RtcManager {
public:
    RtcManager();
    void begin(QueueHandle_t queue);

private:
    static void rtcTask(void* pvParameters);

    QueueHandle_t eventQueue;
};

#endif
