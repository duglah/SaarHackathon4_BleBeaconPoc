#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <BLEAdvertising.h>
#include <Arduino.h>
#include <vector>
#include <functional>
namespace BLE
{
    // #define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"
    static BLEUUID SERVICE_UUID("4fafc201-1fb5-459e-8fcc-c5c9c331914b");

    TaskHandle_t task1;
    BLEAdvertising *pAdvertising;
    BLEAdvertisementData oAdvertisementData = BLEAdvertisementData();
    BLEAdvertisementData oScanResponseData = BLEAdvertisementData();
    uint8_t hueToPublish;

    void Task1code(void *pvParameters)
    {
        for (;;)
        {
            auto res = BLEDevice::getScan()->start(1, false);
            std::vector<BLEAdvertisedDevice> v(res.getCount());
            for (auto i = 0; i < res.getCount(); i++)
            {
                auto device = res.getDevice(i);
                v.push_back(device);

                auto payload = device.getPayload();
                auto serviceData = device.getServiceData();
                auto payloadLength = device.getPayloadLength();

                char str[payloadLength + 1];
                // memcpy(str, payload, payloadLength);
                // for(int i = 0; i < payloadLength; payloadLength++) {
                //     str[i] = payload[10 + i];
                // }
                str[payloadLength] = '\0';

                Serial.printf("Payload: %s\n", str);
                Serial.printf("Payload length: %d\n", payloadLength);
                Serial.printf("ServiceData: %s\n", serviceData);
            }
            auto callback = *static_cast<std::function<void(std::vector<BLEAdvertisedDevice>)> *>(pvParameters);
            callback(v);
            BLEDevice::getScan()->clearResults();
        }
    }

    template <typename T>
    void setup(T *callback, long device_id)
    {
        char buf[25];
        snprintf(buf, sizeof(buf), "St Martins Laterne %lu", device_id);
        BLEDevice::init(buf);
        // BLEDevice::init("St Martins Laterne");
        BLEServer *pServer = BLEDevice::createServer();
        BLEService *pService = pServer->createService(SERVICE_UUID);
        // BLECharacteristic *pCharacteristic = pService->createCharacteristic(
        //     CHARACTERISTIC_UUID,
        //     BLECharacteristic::PROPERTY_READ);

        // pCharacteristic->setValue("Hello World");
        pService->start();
        pAdvertising = BLEDevice::getAdvertising();
        pAdvertising->setScanResponse(true);
        pAdvertising->addServiceUUID(SERVICE_UUID);
        pAdvertising->setScanResponse(true);
        pAdvertising->setMinPreferred(0x06);
        pAdvertising->setMinPreferred(0x12);

        oScanResponseData.setFlags(0x06); // GENERAL_DISC_MODE 0x02 | BR_EDR_NOT_SUPPORTED 0x04
        oScanResponseData.setCompleteServices(SERVICE_UUID);
        oScanResponseData.setServiceData(SERVICE_UUID, "Test123");
        oAdvertisementData.setName("TLMBeacon");
        pAdvertising->setAdvertisementData(oAdvertisementData);
        pAdvertising->setScanResponseData(oScanResponseData);

        BLEDevice::startAdvertising();

        BLEScan *pBLEScan = BLEDevice::getScan();
        pBLEScan->setInterval(1349);
        pBLEScan->setWindow(449);
        pBLEScan->setActiveScan(true);

        xTaskCreatePinnedToCore(
            &Task1code, /* Task function. */
            "Task2",    /* name of task. */
            10000,      /* Stack size of task */
            callback,   /* parameter of the task */
            1,          /* priority of the task */
            &task1,     /* Task handle to keep track of created task */
            1);         /* pin task to core 1 */
    }

    void setHue(uint8_t hue)
    {
        oAdvertisementData.addData("Hue: " + hue);
    }
}