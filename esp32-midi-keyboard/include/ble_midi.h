#ifndef _BLE_MIDI_H_
#define _BLE_MIDI_H_

#include <NimBLEDevice.h>

#define SERVICE_UUID "03B80E5A-EDE8-4B33-A751-6CE34EC4C700"        // MIDI BLE Service
#define CHARACTERISTIC_UUID "7772E5DB-3868-4112-A1A9-F2669D106BF3" // MIDI BLE Characteristic

extern bool deviceConnected; // Make this accessible

extern NimBLECharacteristic *midiCharacteristic;

class MyServerCallbacks : public NimBLEServerCallbacks
{
public:
    void onConnect(NimBLEServer *pServer);
    void onDisconnect(NimBLEServer *pServer);
};

void sendMessage(uint8_t *message, uint8_t messageSize);
void sendNoteOn(uint8_t note);
void sendNoteOff(uint8_t note);

void initBleMidi(const String &deviceName); // New init function prototype

#endif