#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEServer.h>

BLEServer *pServer = nullptr;
BLECharacteristic *pTemperatureCharacteristic = nullptr;
bool deviceConnected = false;

// UUIDs para o serviço e a característica
#define SERVICE_UUID        "00001809-0000-1000-8000-00805F9B34FB"
#define TEMPERATURE_UUID    "00002A1C-0000-1000-8000-00805F9B34FB"

class MyServerCallbacks : public BLEServerCallbacks {
  void onConnect(BLEServer *pServer) {
    deviceConnected = true;
  };

  void onDisconnect(BLEServer *pServer) {
    deviceConnected = false;
  }
};

void setup() {
  Serial.begin(115200);

  // Inicializar BLE
  BLEDevice::init("ESP32_Temperature_Sensor");
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  BLEService *pService = pServer->createService(SERVICE_UUID);

  // Característica de temperatura
  pTemperatureCharacteristic = pService->createCharacteristic(
                                 TEMPERATURE_UUID,
                                 BLECharacteristic::PROPERTY_READ |
                                 BLECharacteristic::PROPERTY_NOTIFY
                               );

  pService->start();

  BLEAdvertising *pAdvertising = pServer->getAdvertising();
  pAdvertising->start();
}

void loop() {
  if (deviceConnected) {
    float temperature = 25.0;  // Substitua por uma leitura real do sensor de temperatura

    uint8_t tempData[4];
    uint16_t tempInMilliDegrees = temperature * 100;
    tempData[0] = tempInMilliDegrees & 0xff;
    tempData[1] = (tempInMilliDegrees >> 8) & 0xff;
    tempData[2] = 0;  // Exponente (0 indica que o valor está em graus Celsius)
    tempData[3] = 0;  // Flags (0 indica que o valor está em graus Celsius)

    pTemperatureCharacteristic->setValue(tempData, 4);
    pTemperatureCharacteristic->notify();
  }
  delay(2000);  // Atualizar a cada 2 segundos
}
