#ifndef SYSTEM_EVENT_H
#define SYSTEM_EVENT_H

enum class EventType {
    ButtonShortPress,
    ButtonLongPress,
    ToggleLayout,
    StartRecording,
    StopRecording,
    HeartRateUpdate,
    RRIntervalUpdate,
    HeartRateDisconnected,
    CadenceUpdate,
    CadenceDisconnected,
    SpeedUpdate,
    SpeedDisconnected,
    BuzzerToneStartRecording,
    BuzzerToneStopRecording,
    BuzzerToneToggleLayout,
};

enum class EventTarget {
    Display,
    Buzzer,
    All
};

struct SystemEvent {
    EventType type;
    EventTarget target;
    union {
        float floatValue;  // For heart rate, speed, etc.
        bool boolValue;    // For recording, connection, etc.
    };
};

#endif
