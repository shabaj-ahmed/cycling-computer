# Project overview
This project is a modular, open-source cycling computer designed for riders who want to monitor and log performance without relying on smartphone. The device is built to operate reliably in outdoor environments while remaining hackable and extensible for developers.

The system is built around the ESP32-S3 platform and uses FreeRTOS to manage concurrent tasks such as sensor data acquisition, display updates, and SD card logging. It integrates:
 * BLE heart rate monitoring (Polar H10),
 * GPS-based tracking,
 * Speed and cadence sensors (Hall effect),
 * IMU for elevation and motion data.

All collected data is logged to an SD card for post-ride analysis. The UI is operated via physical buttons, and the system is designed for fully offline use, with automatic sensor reconnection to ensure robustness during rides.

# System Requirements
This section outlines the design requirements for the cycling computer, focusing on what users need from the system and how the system should behave. It is divided into three parts: user requirements that describe the goals from the user's perspective, functional requirements that specify what the system shall do, and non-functional requirements that define the quality attributes of the system.

##User Requirement
1. Core Functionality
  * UR1: As a user, I want the device to display real-time cycling data, including speed, cadence, heart rate, distance, and elevation.
  * UR2: As a user, I want the device to record GPS data so I can view my travelled route later.
  * UR3: As a user, I want to know the current time and view the duration of the current ride session on the display.
  * UR4: As a user, I want gear suggestions based on speed and cadence so I can maintain consistent effort.
2. Device Operation and Control
  * UR5: As a user, I want to use a single physical button to cycle through display pages.
  * UR6: As a user, I want the system to automatically connect to all peripherals when powered on.
  * UR7: As a user, I want to start ride recording with a short button press and stop it with a long press so I can control when my activity begins and ends.
  * UR8: As a user, I want to receive visual feedback when ride recording starts or stops.
3. Data Logging and Connectivity
  * UR9: As a user, I want all sensor data to be logged to an SD card for post-ride analysis.
  * UR10: As a user, I want the system to reconnect to the heart rate sensor automatically if the connection is lost.
  * UR11: As a user, I want feedback if a sensor (e.g., heart rate, GPS) disconnects or fails during the ride.
4. Environment and Usability
  * UR12: As a user, I want to operate the system offline without a phone or internet connection.
  * UR13: As a user, I want the device to be splash-proof and able to withstand light rain and sweat so I can ride in various weather conditions.
  * UR14: As a user, I want to be able to operate the device using gloves or with wet hands so it remains usable during real-world riding scenarios.

## Functional Requirements
1. Data collection
  * FR1: The system shall acquire heart rate data from the Polar H10 sensor using BLE and Heart Rate Service UUID 0x180D.
  * FR2: The system shall read wheel rotation and pedal cadence using two Hall effect sensors and use them to calculate speed, RPM, and distance.
  * FR3: The system shall read GPS data and compute speed, location, and elevation changes.
  * FR4: The system shall read orientation and motion data from an onboard IMU sensor.
  * FR5: The system shall log all collected data to an SD card in a structured format (e.g., SQLite or CSV).
  * FR6: The system shall timestamp all recorded data using RTC or GPS time.
2. User Interface
  * FR7: The system shall present data across multiple UI pages on an LCD screen.
  * FR8: The system shall use a button interrupt or polling to change the current display page.
  * FR9: The system shall provide a visual indicator if any key sensor (HRM, GPS, SD, Hall sensor) is disconnected or not detected.
  * FR10: The system shall monitor battery voltage and warn the user when the battery is low (if battery monitoring hardware is available).
  * FR11: The system shall allow the user to start data logging using a short press on Button A.
  * FR12: The system shall stop data logging using a long press on Button A.
  * FR13: The system shall display a clear indication on the screen when logging has started or stopped (e.g., status icon or page change).
3. Task Management & Connectivity
  * FR14: The system shall use FreeRTOS to manage concurrent tasks for data collection, display updates, and logging.
  * FR15: The system shall attempt to reconnect to the BLE heart rate sensor if the connection is lost.
  * FR16: The system shall initiate data collection and BLE scanning automatically at startup.

## Non-Functional Requirements
1. Performance
  * NFR1: The display shall update at a minimum rate of 1 Hz.
  * NFR2: BLE connection latency shall not exceed 1 second under normal conditions.
  * NFR3: SD card writes shall occur at intervals of 1–2 seconds to avoid blocking the system.
2. Usability
  * NFR4: The system shall not require user interaction to begin operation after powering on and data collection being started.
  * NFR5: The display shall remain readable in direct sunlight or bright outdoor environments.
  * NFR6: The system shall recover gracefully from unexpected reboots and continue logging without user input.
3. Maintainability
  * NFR7: The software shall be organised using object-oriented C++ design, with one class per sensor or module.
  * NFR8: The system shall be modular and allow individual components (e.g., display, logging, HR sensor) to be tested independently.
  * NFR9: New sensors (e.g., barometer) or features (e.g., gear suggestion logic) should be easy to integrate with minimal refactoring.
  * NFR10: The system shall prevent data loss or corruption in the event of unexpected power loss during logging.
4. Safety and reliability
  * NFR11: The system shall not overheat or draw current beyond safe operating limits during normal operation.
5. Environmental Durability
  * NFR10: The device enclosure protects against splashes from rain and sweat during use.
  * NFR11: All buttons and external interfaces shall remain operational when wet and provide tactile feedback that is usable with cycling gloves.

# Arduino Libraries
* NimBLEDevice-Arduino

## Arduino IDE Setup
### Board Installed
* esp32 by Espressif Systems (v2.0.14)

### Board Settings
* Board: ESP32S3 Dev Module
* USB CDC On Boot: Enabled
* CPU Frequency: 240MHz (WiFi)
* Flash Mode: QIO 80MHz
* Flash Size: 8MB (64Mbit)
* PSRAM: Disabled
* Upload Mode: UART0 / Hardware CDC
* Upload Speed: 921600
* USB Mode: Hardware CDC and JTAG
* Partition Scheme: Default 4MB with spiffs

# Software Architecture

## HeartRateSensor Module

This module manages communication with a Polar H10 heart rate monitor using BLE via the NimBLE library.

### Responsibilities
- Scan for BLE devices and identify Polar H10
- Connect and subscribe to heart rate notifications
- Extract BPM and RR intervals from notifications
- Handle automatic reconnection when disconnected
- Provide access to the latest data via getter methods

### Used in
- Cycling computer project (with GPS, cadence, display, SD card logger)

### Dependencies
- NimBLE-Arduino (BLE library)
- FreeRTOS (task scheduling on ESP32)

### Author
Shabaj Ahmed, July 2025
