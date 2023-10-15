#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <Arduino.h>
#include <vector>
#include <functional>

namespace BLE
{
    // See the following for generating UUIDs:
    // https://www.uuidgenerator.net/
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"
    static BLEUUID SERVICE_UUID("4fafc201-1fb5-459e-8fcc-c5c9c331914b");

    // Task1code: blinks an LED every 1000 ms
    void Task1code(void *pvParameters)
    {
        Serial.print("Task1 running on core ");
        for (;;)
        {
            auto res = BLEDevice::getScan()->start(1, false);
            std::vector<BLEAdvertisedDevice> v(res.getCount());
            for (auto i = 0; i < res.getCount(); i++) {
                v.push_back(res.getDevice(i));
            }
            auto callback = *static_cast<std::function<void(std::vector<BLEAdvertisedDevice>)>*>(pvParameters);
            callback(v);
            BLEDevice::getScan()->clearResults();
            Serial.println("Finished Scanning");
        }
    }

    TaskHandle_t task1;

    template <typename T>
    void setup(T* callback)
    {
        BLEDevice::init("Long name works now");
        BLEServer *pServer = BLEDevice::createServer();
        BLEService *pService = pServer->createService(SERVICE_UUID);
        BLECharacteristic *pCharacteristic = pService->createCharacteristic(
            CHARACTERISTIC_UUID,
            BLECharacteristic::PROPERTY_READ |
                BLECharacteristic::PROPERTY_WRITE);

        pCharacteristic->setValue("Hello World says Neil");
        pService->start();
        // BLEAdvertising *pAdvertising = pServer->getAdvertising();  // this still is working for backward compatibility
        BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
        pAdvertising->addServiceUUID(SERVICE_UUID);
        pAdvertising->setScanResponse(true);
        pAdvertising->setMinPreferred(0x06); // functions that help with iPhone connections issue
        pAdvertising->setMinPreferred(0x12);
        BLEDevice::startAdvertising();
        Serial.println("Characteristic defined! Now you can read it in your phone!");

        BLEScan *pBLEScan = BLEDevice::getScan();
        pBLEScan->setInterval(1349);
        pBLEScan->setWindow(449);
        pBLEScan->setActiveScan(true);


        xTaskCreatePinnedToCore(
            &Task1code, /* Task function. */
            "Task2",   /* name of task. */
            10000,     /* Stack size of task */
            callback,      /* parameter of the task */
            1,         /* priority of the task */
            &task1, /* Task handle to keep track of created task */
            1);        /* pin task to core 1 */
    }
}
