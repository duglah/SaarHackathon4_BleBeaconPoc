/*
    Based on Neil Kolban example for IDF: https://github.com/nkolban/esp32-snippets/blob/master/cpp_utils/tests/BLE%20Tests/SampleServer.cpp
    Ported to Arduino ESP32 by Evandro Copercini
    updates by chegewara
*/

#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <WS2812FX.h>

#define LED_COUNT 12
#define LED_PIN 16

WS2812FX ws2812fx = WS2812FX(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);



// See the following for generating UUIDs:
// https://www.uuidgenerator.net/
       
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"
static BLEUUID SERVICE_UUID("4fafc201-1fb5-459e-8fcc-c5c9c331914b");

/**
 * Scan for BLE servers and find the first one that advertises the service we are looking for.
 */
class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
  public:
  uint64_t time = 0;

 /**
   * Called for each advertising BLE server.
   */
  void onResult(BLEAdvertisedDevice advertisedDevice) {

    // We have found a device, let us now see if it contains the service we are looking for.
    if (advertisedDevice.haveServiceUUID() && advertisedDevice.isAdvertisingService(SERVICE_UUID)) {
      // store time
      time = millis();

      Serial.print("BLE Advertised Device found: ");
      Serial.println(advertisedDevice.toString().c_str());
      Serial.print("Rssi: ");
      Serial.println(advertisedDevice.getRSSI());
      // BLEDevice::getScan()->stop();
      // myDevice = new BLEAdvertisedDevice(advertisedDevice);
      // doConnect = true;
      // doScan = true;

    } // Found our server
  } // onResult
};

MyAdvertisedDeviceCallbacks* advertiseCallbacks;


void setup() {
  Serial.begin(9600);
  Serial.println("Starting BLE work!");

 ws2812fx.init();
 ws2812fx.setBrightness(100);
 ws2812fx.setSpeed(200);
 ws2812fx.setMode(FX_MODE_RAINBOW_CYCLE);
 ws2812fx.start();

  BLEDevice::init("Long name works now");
  BLEServer *pServer = BLEDevice::createServer();
  BLEService *pService = pServer->createService(SERVICE_UUID);
  BLECharacteristic *pCharacteristic = pService->createCharacteristic(
                                         CHARACTERISTIC_UUID,
                                         BLECharacteristic::PROPERTY_READ |
                                         BLECharacteristic::PROPERTY_WRITE
                                       );

  pCharacteristic->setValue("Hello World says Neil");
  pService->start();
  // BLEAdvertising *pAdvertising = pServer->getAdvertising();  // this still is working for backward compatibility
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x06);  // functions that help with iPhone connections issue
  pAdvertising->setMinPreferred(0x12);
  BLEDevice::startAdvertising();
  Serial.println("Characteristic defined! Now you can read it in your phone!");




  Serial.println("Starting Arduino BLE Client application...");
   // Retrieve a Scanner and set the callback we want to use to be informed when we
  // have detected a new device.  Specify that we want active scanning and start the
  // scan to run for 5 seconds.
  BLEScan* pBLEScan = BLEDevice::getScan();
  advertiseCallbacks = new MyAdvertisedDeviceCallbacks();
  pBLEScan->setAdvertisedDeviceCallbacks(advertiseCallbacks);
  pBLEScan->setInterval(1349);
  pBLEScan->setWindow(449);
  pBLEScan->setActiveScan(true);
  pBLEScan->start(5, false);
}

void loop() {
  if (millis() == 1) {
    ws2812fx.service();
  }

  if (millis() - advertiseCallbacks->time < 10000) {
    ws2812fx.service();
  }

  if (millis() % 10000 == 0) {
    Serial.println("Scanning");
    BLEDevice::getScan()->start(1);
    Serial.println("Finished Scanning");
  }
  // put your main code here, to run repeatedly:
  // delay(10000); 
}