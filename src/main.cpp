#include <Arduino.h>
#include <WS2812FX.h>
#include <mutex>
#include "bluetooth.hpp"
#include "led.hpp"
#include <numeric>

// stores last time device received ble signal and signal strength from device with service uuid
struct BleTime {
  private:
  std::mutex mutex;
  uint64_t time;
  int rssi;

  public:
    void set(int rssi) {
      std::lock_guard<std::mutex> lock(mutex);
      this->time = millis();
      this->rssi = rssi;
    }

    int64_t getTime() {
      std::lock_guard<std::mutex> lock(mutex);
      return time;
    }

    int getMaxRssi() {
      std::lock_guard<std::mutex> lock(mutex);
      return rssi;
    }
} bleTime;

std::function<void(std::vector<BLEAdvertisedDevice>)> handleBleDevices = [&](std::vector<BLEAdvertisedDevice> v) {
  int maxDeviceRssi = -1000;

  for (auto& device: v) {
    if (device.haveServiceUUID() && device.isAdvertisingService(BLE::SERVICE_UUID)) {
      Serial.printf("BLE Advertised Device found: %s", device.getName());
      Serial.printf("Rssi: %d\n", device.getRSSI());
      int rssi = device.getRSSI();
      maxDeviceRssi = max(rssi, maxDeviceRssi);
    }
  }

  if (maxDeviceRssi != -1000) {
    bleTime.set(maxDeviceRssi);
  } else {
    Serial.println("RSS 0");
  }
};


void setup() {
  Serial.begin(9600);
  Serial.println("Starting BLE work!");
  LED::setup();
  BLE::setup(&handleBleDevices);

  Serial.println("Starting Arduino BLE Client application...");
}


void loop() {
  bool deviceIsClose = millis() - bleTime.getTime() < 1500;
  if (deviceIsClose) {
    Serial.println("IS Close");
  }
  LED::loop(deviceIsClose, bleTime.getMaxRssi());
}