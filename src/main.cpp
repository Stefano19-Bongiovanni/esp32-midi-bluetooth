#include <Arduino.h>
#include <NimBLEDevice.h>

#define ROWS 8
#define COLUMNS 8

bool noteMatrix[ROWS * COLUMNS] = {false};

const int pairButtonPin = 2;

#define SERVICE_UUID "03B80E5A-EDE8-4B33-A751-6CE34EC4C700"        // MIDI BLE Service
#define CHARACTERISTIC_UUID "7772E5DB-3868-4112-A1A9-F2669D106BF3" // MIDI BLE Characteristic

bool deviceConnected = false;
NimBLECharacteristic *midiCharacteristic;

class MyServerCallbacks : public NimBLEServerCallbacks
{
  void onConnect(NimBLEServer *pServer)
  {
    deviceConnected = true;
    Serial.println("Device connected.");
  }

  void onDisconnect(NimBLEServer *pServer)
  {
    deviceConnected = false;
    Serial.println("Device disconnected.");
  }
};

void sendMessage(uint8_t *message, uint8_t messageSize)
{
  uint8_t packet[messageSize + 2];

  auto t = millis();
  uint8_t headerByte = (1 << 7) | ((t >> 7) & ((1 << 6) - 1));
  uint8_t timestampByte = (1 << 7) | (t & ((1 << 7) - 1));

  packet[0] = headerByte;
  packet[1] = timestampByte;
  for (int i = 0; i < messageSize; i++)
    packet[i + 2] = message[i];

  midiCharacteristic->setValue(packet, messageSize + 2);
  midiCharacteristic->notify();
}

void sendNoteOn(uint8_t note)
{
  uint8_t midiData[] = {144, note, 127};
  sendMessage(midiData, 3);
}

void sendNoteOff(uint8_t note)
{
  uint8_t midiData[] = {128, note, 127};
  sendMessage(midiData, 3);
}

void setup()
{
  Serial.begin(115200);
  pinMode(pairButtonPin, INPUT_PULLUP);

  // BLE init
  NimBLEDevice::init("ESP32-BLE-MIDI");
  NimBLEServer *pServer = NimBLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  // MIDI Service
  NimBLEService *pMidiService = pServer->createService(SERVICE_UUID);
  midiCharacteristic = pMidiService->createCharacteristic(
      CHARACTERISTIC_UUID,
      NIMBLE_PROPERTY::READ |
          NIMBLE_PROPERTY::WRITE |
          NIMBLE_PROPERTY::NOTIFY);

  pMidiService->start();
  Serial.println("pMidiService started");

  // Start advertising
  NimBLEAdvertising *pAdvertising = NimBLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(true);

  pAdvertising->start();
  Serial.println("Waiting a client connection to notify...");
}

void loop()
{
  if (deviceConnected)
  {
    sendNoteOn(0x3C);
    Serial.println("Note on");
    delay(1000);
    sendNoteOff(0x3C);
    Serial.println("Note off");
    delay(2000);
  }
}