#include <NimBLEDevice.h>
#include <MIDI.h>

// MIDI configuration
MIDI_CREATE_DEFAULT_INSTANCE();
#define RX_PIN 16 // RX pin (optional, for receiving MIDI from 5-pin port)
#define TX_PIN 17 // TX pin (for sending MIDI to the 5-pin port)

// BLE configuration
NimBLEAdvertisedDevice *bleMIDIAdvertisedDevice;
NimBLEClient *pClient;
NimBLERemoteService *pService;
NimBLERemoteCharacteristic *pCharacteristic;
bool deviceConnected = false;
std::string targetMAC = "xx:xx:xx:xx:xx:xx"; // Replace with BLE MIDI device MAC
HardwareSerial MIDIOut(1);                   // UART1 for MIDI Out (5-pin)

void setup()
{
  Serial.begin(115200);
  MIDIOut.begin(31250, SERIAL_8N1, RX_PIN, TX_PIN);

  // BLE Setup
  NimBLEDevice::init("");
  NimBLEDevice::setPower(ESP_PWR_LVL_P9);
  NimBLEScan *pScan = NimBLEDevice::getScan();
  pScan->setAdvertisedDeviceCallbacks(new AdvertisedDeviceCallbacks());
  pScan->setInterval(45);
  pScan->setWindow(15);
  pScan->setActiveScan(true);
  pScan->start(0); // Scan indefinitely
}

void loop()
{
  // Handle MIDI relay
  if (deviceConnected)
  {
    if (pCharacteristic->canRead())
    {
      std::string midiData = pCharacteristic->readValue();
      relayMIDITo5Pin(midiData);
    }
  }

  // Check BLE connection status and attempt reconnection if disconnected
  if (!deviceConnected)
  {
    connectToDevice();
  }
}

// Callback for BLE devices found
class AdvertisedDeviceCallbacks : public NimBLEAdvertisedDeviceCallbacks
{
  void onResult(NimBLEAdvertisedDevice *advertisedDevice)
  {
    if (advertisedDevice->getAddress().toString() == targetMAC)
    {
      bleMIDIAdvertisedDevice = advertisedDevice;
      advertisedDevice->getScan()->stop();
    }
  }
};

// Function to connect to the BLE MIDI device
void connectToDevice()
{
  if (bleMIDIAdvertisedDevice != nullptr)
  {
    pClient = NimBLEDevice::createClient();
    pClient->connect(bleMIDIAdvertisedDevice);
    pService = pClient->getService("UUID_of_MIDI_Service");                       // Set the correct service UUID
    pCharacteristic = pService->getCharacteristic("UUID_of_MIDI_Characteristic"); // Set the correct characteristic UUID
    deviceConnected = true;
  }
}

// Relay BLE MIDI to 5-pin MIDI port
void relayMIDITo5Pin(const std::string &midiData)
{
  for (char byte : midiData)
  {
    Serial.print("sending byte");
    MIDIOut.write(byte); // Relay each byte to MIDI port
  }
}