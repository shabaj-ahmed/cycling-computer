/**
 * @file HeartRateSensor.h
 * @brief BLE heart rate sensor interface for Polar H10 using NimBLE and FreeRTOS
 *
 * This class handles BLE scanning, connecting and subscribing to HR notifications,
 * and exposing heart rate and the length of a ventricular cardiac cycle (RR intervals)
 * to the rest of the system.
 *
 * Intended for embedded health-monitoring in a cycling computer.
 */

#ifndef HEARTRATESENSOR_H
#define HEARTRATESENSOR_H

#include <NimBLEDevice.h>
#include <vector>
#include "SystemEvent.h"

/**
 * @brief Class to manage a BLE connection to a Polar H10 Heart Rate Sensor.
 * 
 * This class scans for the Polar H10, connects, subscribes to heart rate notifications,
 * and provides access to real-time heart rate and R-R interval data.
 */
class HeartRateSensor {
public:
    /**
     * @brief Constructor. Initialises internal state.
     */
    HeartRateSensor();

    /**
     * @brief Destructor. Cleans up BLE client and callbacks.
     */
    ~HeartRateSensor();

    /**
     * @brief Starts BLE scanning. Call once during setup.
     */
    void begin(QueueHandle_t queue);

    /**
     * @brief Attempts to connect and subscribe to Polar H10 heart rate data.
     * 
     * Call this periodically from a FreeRTOS task.
     */
    void scanAndConnect();

    /**
     * @brief Returns the latest heart rate in BPM.
     */
    float getHeartRate() const;

    /**
     * @brief Returns the latest R-R intervals in milliseconds.
     */
    std::vector<float> getRRIntervals() const;

    /**
     * @brief Returns true if the device is currently connected.
     */
    bool isConnected() const;

private:
    QueueHandle_t eventQueue;
    
    // BLE objects and heart rate data
    float bpm;
    bool connected;
    NimBLEAdvertisedDevice* foundDevice;
    NimBLEClient* client;
    std::vector<float> rrIntervals;

    // Callback object for scanning
    NimBLEScanCallbacks* scanCallbacks = nullptr;

    // Singleton instance for BLE callback access
    static HeartRateSensor* instance;

    /**
     * @brief Internal BLE callback: handles new data notification.
     */
    static void onNotify(
        NimBLERemoteCharacteristic* characteristic,
        uint8_t* data, size_t length, bool isNotify
    );

    static void heartRateTask(void* pvParameters);

    /**
     * @brief Parses heart rate and RR interval data from BLE.
     */
    void processHeartRateData(uint8_t* data, size_t length);

    /**
     * @brief BLE scan result handler. Nested class used internally.
     */
    class ScanCallbacks : public NimBLEScanCallbacks {
    public:
        ScanCallbacks(HeartRateSensor* parent);
        void onResult(const NimBLEAdvertisedDevice* device) override;

    private:
        HeartRateSensor* parent;
    };
};

#endif