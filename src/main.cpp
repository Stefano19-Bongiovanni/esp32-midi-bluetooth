#include <Arduino.h>
#include <NimBLEDevice.h>
#include "ble_midi.h"

#define NOTE_STARTS_FROM 3
#define NOTE_THRESHOLD 3800
#define ROWS 8
#define COLUMNS 8

const int PINS_ROWS[] = {12, 14, 27, 26, 25, 33, 32, 21}; // G35 --> G21
const int PINS_COLUMNS[] = {18, 5, 17, 16, 4, 19, 2, 15}; // G0 --> G19

/*
#define ROWS 2
#define COLUMNS 2
const int PINS_ROWS[] = {
    12,
    14,
};
const int PINS_COLUMNS[] = {
    26,
    27,
}; */

bool notesMatrix[ROWS * COLUMNS] = {false};

const int pairButtonPin = 2;

void setup()
{
  Serial.begin(115200);

  // pinMode(pairButtonPin, INPUT_PULLUP);
  initBleMidi("EKO Panda61");

  for (int i = 0; i < COLUMNS; i++)
  {
    pinMode(PINS_COLUMNS[i], INPUT_PULLDOWN);
  }
}
uint8_t convertNote(int iNote)
{
  return iNote + 21 + NOTE_STARTS_FROM;
}

void setChange(int iNote, bool value)
{
  /* Serial.print("CHANGE ON NOTE ");
  Serial.print(iNote);
  Serial.print(" VALUE: ");
  Serial.println(value); */
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
        setChange(matrixIndex, value);
    }
  }
  // Serial.print("\n\n");

  // int row = (ROWS * COLUMNS) / iNote;
  // int column = (ROWS * COLUMNS) % iNote;
}

void loop()
{

  // readKeys();
  delay(10);
  /*  if (deviceConnected)
   {
     sendNoteOn(0x3C);
     Serial.println("Note on");
     delay(1000);
     sendNoteOff(0x3C);
     Serial.println("Note off");
     delay(2000);
   } */
  delay(1);
}