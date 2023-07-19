#include "MIDIUSB.h"
#include "FastLED.h"

#define DEBOUNCEVALUE 500  // time between two button presses required debouncetime

// Add definition of Buttons and corresponding LED here
#define BUTTON1PIN 2
#define BUTTON2PIN 3
#define BUTTON3PIN 4

#define LEDDATAPIN 5  //LED PIN für Neopixel

#define NUM_LEDS 3

uint8_t buttons[] = { BUTTON1PIN, BUTTON2PIN, BUTTON3PIN };  // Add BUTTONXPIN to list


// Automatisch generierte Arrays/Variablen
CRGB leds[NUM_LEDS];
bool states[sizeof(buttons)] = {};
long debounce[sizeof(buttons)] = {};  // Array to debounce the buttons

// Eigene Funktionen
void controlChange(byte channel, byte control, byte value) {  // Funktion für das Ansteuern von Midi-Devices
  midiEventPacket_t event = { 0x0B, 0xB0 | channel, control, value };
  MidiUSB.sendMIDI(event);
}

void sendMidiMessage(uint8_t ccCommand, bool state)  // ccCommand is the desired cc , state is the current state of the switch
{
  if (state) {
    controlChange(ccCommand, 10, 127);  // Turn On
  } else {
    controlChange(ccCommand, 10, 1);  // Turn Off
  }
  MidiUSB.flush();  // Send Midi Message
}



void setup() {
  for (size_t i = 0; i < sizeof(buttons); i++)  // set pinModes for buttons and leds
  {
    pinMode(buttons[i], INPUT_PULLUP);
    //pinMode(leds[i], OUTPUT);
  }

  for (size_t i = 0; i < sizeof(buttons); i++)  // Set every state off so no effect will be activated by startup
  {
    states[i] = false;
    sendMidiMessage(i, states[i]);
  }
  FastLED.addLeds<NEOPIXEL, LEDDATAPIN>(leds, NUM_LEDS);  // GRB ordering is assumed
  leds[0] = CRGB::Red;
  leds[1] = CRGB::Green;
  leds[2] = CRGB::Blue;
  FastLED.show();
}

void controlLED(uint8_t led, uint8_t state) {  // Funktion zum Ansteuern der LEDs
  switch (led) {
    case 0:  // LED 1
      if (state) {
        leds[led] = CRGB::Red;
      } else {
        leds[led] = CRGB::Black;  // Black: LED leuchtet nicht
      }
      break;
    case 1:  // LED 2
      if (state) {
        leds[led] = CRGB::Green;
      } else {
        leds[led] = CRGB::Black;
      }
      break;
    case 2:  // LED 3
      if (state) {
        leds[led] = CRGB::Blue;
      } else {
        leds[led] = CRGB::Black;
      }
      break;
  }
  FastLED.show();
}

void loop() {
  for (size_t i = 0; i < sizeof(buttons); i++) {
    if (millis() - debounce[i] > DEBOUNCEVALUE)  // check if button is ready for the next press
    {
      if (!digitalRead(buttons[i]))  // read if button is pressed
      {
        states[i] = !states[i];                 // change state
        uint8_t ccMessage = i + 1;              // can't be 0
        sendMidiMessage(ccMessage, states[i]);  // send midi CC message

        controlLED(i, states[i]);  // Set LED
        debounce[i] = millis();    // Debounce
      }
    }
  }
}