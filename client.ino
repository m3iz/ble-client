#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>

// Замените на UUID вашего устройства BLE и характеристик
#define SERVICE_UUID        "180D"
#define CHARACTERISTIC_UUID "180D"

BLEClient* pClient;
BLEScan* pBLEScan;
boolean connected = false;
BLERemoteCharacteristic* pRemoteCharacteristic;

class MyClientCallback : public BLEClientCallbacks {
    void onConnect(BLEClient* pclient) {
        connected = true;
    }

    void onDisconnect(BLEClient* pclient) {
        connected = false;
    }
};

void setup() {
  Serial.begin(115200);
  BLEDevice::init("ESP32 Client");
  pClient  = BLEDevice::createClient();
  pClient->setClientCallbacks(new MyClientCallback());

  // Сканирование устройства BLE
  pBLEScan = BLEDevice::getScan(); // Создание нового экземпляра BLEScan
  pBLEScan->setActiveScan(true); // Сканирование активно (не прослушивание)
  BLEScanResults foundDevices = pBLEScan->start(5); // Сканирование в течение 5 секунд
  for (int i = 0; i < foundDevices.getCount(); i++) {
    if (foundDevices.getDevice(i).getName() == "ESP32") { // Замените на имя вашего устройства BLE
      BLEAdvertisedDevice advertisedDevice = foundDevices.getDevice(i);
      pClient->connect(&advertisedDevice); // Подключение к устройству
      Serial.println("Device connected");
      break;
    }
  }
}

void loop() {
  if (connected) {
    BLERemoteService* pRemoteService = pClient->getService(SERVICE_UUID);
    if (pRemoteService) {
      pRemoteCharacteristic = pRemoteService->getCharacteristic(CHARACTERISTIC_UUID);
      if (pRemoteCharacteristic) {
        // Чтение значения характеристики
        std::string value = pRemoteCharacteristic->readValue();
        Serial.println("Characteristic Value: " + String(value.c_str()));

        // Изменение значения характеристики
        pRemoteCharacteristic->writeValue((uint8_t*)"New Value", 9);
      }
    }
  }
  delay(2000); // Подождать 2 секунды перед повторным сканированием
}
