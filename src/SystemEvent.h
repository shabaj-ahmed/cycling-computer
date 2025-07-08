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
};

struct SystemEvent {
    EventType type;
    union {
        float floatValue;  // For heart rate, speed, etc.
        bool boolValue;    // For recording, connection, etc.
    };
};

#endif
