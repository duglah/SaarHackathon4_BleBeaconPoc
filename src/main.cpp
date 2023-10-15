#include <Arduino.h>
#include <FastLED.h>
#include <mutex>
#include "bluetooth.hpp"
#include <numeric>

#define LED_COUNT 12
#define LED_PIN 16

// stores last time device received ble signal and signal strength from device with service uuid
struct BleTime
{
private:
  std::mutex mutex;
  uint64_t time;
  int rssi;

public:
  void set(int rssi)
  {
    std::lock_guard<std::mutex> lock(mutex);
    this->time = millis();
    this->rssi = rssi;
  }

  int64_t getTime()
  {
    std::lock_guard<std::mutex> lock(mutex);
    return time;
  }

  int getMaxRssi()
  {
    std::lock_guard<std::mutex> lock(mutex);
    return rssi;
  }
} bleTime;

std::function<void(std::vector<BLEAdvertisedDevice>)> handleBleDevices = [&](std::vector<BLEAdvertisedDevice> v)
{
  int maxDeviceRssi = -1000;

  for (auto &device : v)
  {
    if (device.haveServiceUUID() && device.isAdvertisingService(BLE::SERVICE_UUID))
    {
      Serial.printf("BLE Advertised Device found: ");
      Serial.printf("Rssi: %d\n", device.getRSSI());
      int rssi = device.getRSSI();
      maxDeviceRssi = max(rssi, maxDeviceRssi);
    }
  }

  if (maxDeviceRssi != -1000)
  {
    bleTime.set(maxDeviceRssi);
  }
};

long device_id;

CRGB leds[LED_COUNT];
unsigned long nextStepSize = 50;
unsigned long nextStepOn;
uint8_t hue;

void setup()
{
  // Start Serial port
  Serial.begin(9600);
  
  // Generate pseudo random number for device id
  randomSeed(analogRead(0));
  device_id = random(255);
  Serial.printf("Starting as St Martins Laterne #%lu\n", device_id);

  // Start FastLED
  FastLED.addLeds<NEOPIXEL, LED_PIN>(leds, LED_COUNT);
 
  // Start BLE Serivce
  BLE::setup(&handleBleDevices, device_id);
}

void loop()
{
  bool deviceRecentlyFound = millis() - bleTime.getTime() < 1500;
  bool deviceIsNear = bleTime.getMaxRssi() > -75;

  // Is other lantern near and was recently found?
  if (deviceRecentlyFound && deviceIsNear)
  {
    // Last update was just now, so we skip
    if(millis() < nextStepOn) {
      return;
    }

    // Set rainbow color
    fill_solid(&(leds[0]), LED_COUNT, CHSV(hue, 208, 255));
    FastLED.show();

    nextStepOn = millis() + nextStepSize;
    hue++;
    BLE::setHue(hue);
  }
  else
  {
    // Blink red!
    fill_solid(&(leds[0]), LED_COUNT, CRGB::Red);
    FastLED.show();
    delay(500);

    fill_solid(&(leds[0]), LED_COUNT, CRGB::Black);
    FastLED.show();
    delay(500);
  }
}