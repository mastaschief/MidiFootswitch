#include "MIDIUSB.h"

#define DEBOUNCEVALUE 500 // time between two button presses required debouncetime

// Add definition of Buttons and corresponding LED here
#define BUTTON1PIN 2
#define LED1PIN 3


uint8_t buttons[] = {BUTTON1PIN}; // Add BUTTONXPIN to list

uint8_t leds[] = {LED1PIN}; // Add LEDXPIN to list

bool states[sizeof(buttons)] = {};

long debounce[sizeof(buttons)] = {}; // Array to debounce the buttons


void controlChange(byte channel, byte control, byte value)
{
  midiEventPacket_t event = {0x0B, 0xB0 | channel, control, value};
  MidiUSB.sendMIDI(event);
}


void sendMidiMessage(uint8_t button, bool state) // button is the BUTTONXPIN, state is the current state of the switch
{
  if (state)
  {
    controlChange(button, 10, 127); // Turn On
  }
  else
  {
    controlChange(button, 10, 1); // Turn Off
  }
  MidiUSB.flush();// Send Midi Message
}


void setup()
{
  for (size_t i = 0; i < sizeof(buttons); i++) // set pinModes for buttons and leds
  {
  pinMode(buttons[i], INPUT_PULLUP);
  pinMode(leds[i], OUTPUT);
  }

  for (size_t i = 0; i < sizeof(buttons); i++) // Set every state off so no effect will be activated by startup
  {
    states[i] = false;
    sendMidiMessage(i, states[i]);
  }
}


void loop()
{
  for (size_t i = 0; i < sizeof(buttons); i++)
  {
    if (millis() - debounce[i] > DEBOUNCEVALUE) // check if button is ready for the next press
    {
      if (!digitalRead(buttons[i])) // read if button is pressed
      {
        states[i] = !states[i]; // change state
        sendMidiMessage(states[i], i); // Send Midi Message
        digitalWrite(leds[i], states[i]); // Set LED
        debounce[i] = millis(); // Debounce
      }
    }
  }
}
