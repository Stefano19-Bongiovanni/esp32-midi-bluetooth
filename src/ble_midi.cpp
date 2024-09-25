#include "ble_midi.h"

bool deviceConnected = false; // Now accessible from other files
NimBLECharacteristic *midiCharacteristic;

void MyServerCallbacks::onConnect(NimBLEServer *pServer)
{
    deviceConnected = true;
    Serial.println("Device connected.");
}

void MyServerCallbacks::onDisconnect(NimBLEServer *pServer)
{
    deviceConnected = false;
    Serial.println("Device disconnected.");
}

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

void initBleMidi(const String &deviceName)
{
    // BLE init
    NimBLEDevice::init(deviceName.c_str()); // Use the string parameter
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
    Serial.println("Waiting for a client connection to notify...");
}
