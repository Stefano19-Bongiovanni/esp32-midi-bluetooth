#include <Arduino.h>
#include <NimBLEDevice.h>
#include "ble_midi.h"

#define NOTE_STARTS_FROM 18
#define ROWS 8
#define COLUMNS 8

#define DEBUG_SERIAL false

const int PINS_ROWS[] = {12, 14, 27, 26, 25, 33, 32, 21}; // G35 --> G21

const int PINS_COLUMNS[] = {18, 4, 19, 2, 15, 16, 17, 5}; // G0 --> G19

bool notesMatrix[ROWS * COLUMNS] = {false};

const int pairButtonPin = 2;

void setup()
{
  Serial.begin(115200);

  // pinMode(pairButtonPin, INPUT_PULLUP);
  if (DEBUG_SERIAL)
    Serial.println("Initializing...");
  initBleMidi("EKO Panda61");
  if (DEBUG_SERIAL)
    Serial.println("Bluetooth initialized");

  for (int i = 0; i < COLUMNS; i++)
  {
    pinMode(PINS_COLUMNS[i], INPUT_PULLUP);
    if (DEBUG_SERIAL)
    {
      Serial.print("COLUMN: ");
      Serial.print(i);
      Serial.print(" PIN: ");
      Serial.println(PINS_COLUMNS[i]);
    }
  }
}
uint8_t convertNote(int iNote)
{
  return iNote + NOTE_STARTS_FROM;
}

void setChange(int iNote, bool value)
{

  notesMatrix[iNote] = value;

  if (deviceConnected)
  {

    if (value)
    {
      sendNoteOn(convertNote(convertNote(iNote)));
    }
    else
    {
      sendNoteOff(convertNote(convertNote(iNote)));
    }
  }
}

void readKeys()
{
  for (int row = 0; row < ROWS; row++)
  {

    // set all pins to INPUT
    for (int i = 0; i < ROWS; i++)
    {
      pinMode(PINS_ROWS[i], INPUT);
    }
    pinMode(PINS_ROWS[row], OUTPUT);
    digitalWrite(PINS_ROWS[row], LOW);
    delayMicroseconds(1000);

    for (int column = 0; column < COLUMNS; column++)
    {
      int matrixIndex = row * COLUMNS + column;
      bool value = !digitalRead(PINS_COLUMNS[column]);

      if (notesMatrix[matrixIndex] != value)
      {
        if (DEBUG_SERIAL)
        {
          Serial.print("R: ");
          Serial.print(row);
          Serial.print(" P: ");
          Serial.print(PINS_ROWS[row]);
          Serial.print(" | C: ");
          Serial.print(column);
          Serial.print(" P: ");
          Serial.print(PINS_COLUMNS[column]);
          Serial.print(" | V: ");
          Serial.println(value);
        }
        setChange(matrixIndex, value);
      }
    }
  }
}

void sendRandomMidi()
{
  sendNoteOn(41);
  Serial.println("Note on");
  delay(2000);
  sendNoteOff(41);
  Serial.println("Note off");
  delay(3000);
}

void loop()
{
  // sendRandomMidi();
  readKeys();

  delay(5);
}