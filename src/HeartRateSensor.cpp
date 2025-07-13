#include "HeartRateSensor.h"
#include "SystemEvent.h"

// extern DisplayManager display;
SystemEvent event = { EventType::HeartRateUpdate };

// Static instance for use in static callbacks
HeartRateSensor* HeartRateSensor::instance = nullptr;

// -----------------------------
// Constructor and Destructor
// -----------------------------

HeartRateSensor::HeartRateSensor() {
  bpm = 0;
  client = nullptr;
  foundDevice = nullptr;
  connected = false;
}

HeartRateSensor::~HeartRateSensor() {
  if (scanCallbacks) {
    delete scanCallbacks;
    scanCallbacks = nullptr;
  }
  if (client) {
    NimBLEDevice::deleteClient(client);
    client = nullptr;
  }
}

// -----------------------------
// Public Methods
// -----------------------------

void HeartRateSensor::begin(QueueHandle_t queue) {
  this->eventQueue = queue;

  instance = this;

  NimBLEDevice::init("");  // Initialize BLE
  NimBLEScan* scanner = NimBLEDevice::getScan();
  scanCallbacks = new ScanCallbacks(this);

  scanner->setScanCallbacks(scanCallbacks);
  scanner->setActiveScan(true);
  scanner->setInterval(45);
  scanner->setWindow(15);
  scanner->start(0, false);  // Non-blocking scan

  // Create the heart rate task
    xTaskCreatePinnedToCore(
        heartRateTask,
        "HeartRateTask",
        2048,
        this,
        1,
        nullptr,
        1
    );
}

void HeartRateSensor::heartRateTask(void* pvParameters) {
    HeartRateSensor* self = static_cast<HeartRateSensor*>(pvParameters);
    while (true) {
        self->scanAndConnect();
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void HeartRateSensor::scanAndConnect() {
  // Reconnect if connection is lost
  if (client && !client->isConnected()) {
    // Serial.println("Lost connection. Resetting client...");
    NimBLEDevice::deleteClient(client);
    client = nullptr;
    foundDevice = nullptr;
    connected = false;

    // Restart scan
    NimBLEScan* scanner = NimBLEDevice::getScan();
    scanner->setScanCallbacks(scanCallbacks);  // already set, but safe
    scanner->start(0, false);  // restart non-blocking scan
    // Serial.println("Restarting BLE scan...");
    return;  // wait for scan to rediscover the device
  }

  if (!foundDevice || client) return;

  client = NimBLEDevice::createClient();
  // Serial.println("Connecting to Polar H10...");

  if (!client->connect(foundDevice)) {
    // Serial.println("Connection failed.");
    NimBLEDevice::deleteClient(client);
    client = nullptr;
    foundDevice = nullptr;
    connected = false;
    return;
  }

  // Serial.println("Connected to Polar H10!");
  connected = true;

  NimBLERemoteService* hrService = client->getService("180D");
  if (!hrService) {
    // Serial.println("Heart Rate service not found.");
    return;
  }

  NimBLERemoteCharacteristic* hrChar = hrService->getCharacteristic("2A37");
  if (!hrChar || !hrChar->canNotify()) {
    // Serial.println("HR characteristic not found or not notifiable.");
    return;
  }

  // Serial.println("Subscribing to heart rate notifications...");
  hrChar->subscribe(true, onNotify);
}

float HeartRateSensor::getHeartRate() const {
  return bpm;
}

std::vector<float> HeartRateSensor::getRRIntervals() const {
  return rrIntervals;
}

bool HeartRateSensor::isConnected() const {
  return connected;
}

// -----------------------------
// Static BLE Notification Handler
// -----------------------------

void HeartRateSensor::onNotify(NimBLERemoteCharacteristic* characteristic, uint8_t* data, size_t length, bool isNotify) {
  if (instance) {
    instance->processHeartRateData(data, length);
  }
}

// -----------------------------
// Private Data Processing
// -----------------------------

void HeartRateSensor::processHeartRateData(uint8_t* data, size_t length) {
  if (length < 2) return;

  uint8_t flags = data[0];
  bool rrPresent = flags & 0x10;
  bpm = data[1];

  // Send BPM event
  SystemEvent bpmEvent = {
                .type = EventType::HeartRateUpdate,
                .target = EventTarget::Display,
                .floatValue = bpm
            };
  xQueueSend(eventQueue, &bpmEvent, 0);

  rrIntervals.clear();
  if (rrPresent) {
    for (int i = 2; i + 1 < length; i += 2) {
      uint16_t rr = data[i] | (data[i + 1] << 8);
      float rr_ms = rr * 1000.0 / 1024.0;
      rrIntervals.push_back(rr_ms);
      // Send separate event for RR intervals
      SystemEvent rrEvent = {
                .type = EventType::RRIntervalUpdate,
                .target = EventTarget::Display,
                .floatValue = rr_ms
            };
      xQueueSend(eventQueue, &rrEvent, 0);
    }
  }
}

// -----------------------------
// Scan Callback Implementation
// -----------------------------

HeartRateSensor::ScanCallbacks::ScanCallbacks(HeartRateSensor* parent) : parent(parent) {}

void HeartRateSensor::ScanCallbacks::onResult(const NimBLEAdvertisedDevice* device) {
  // Serial.print("Found device: ");
  // Serial.println(device->toString().c_str());

  if (device->getName().find("Polar") != std::string::npos ||
    device->isAdvertisingService(NimBLEUUID("180D"))) {
    // Serial.println("Polar H10 Found!");
    parent->foundDevice = const_cast<NimBLEAdvertisedDevice*>(device);
    NimBLEDevice::getScan()->stop();
  }
}
