#include "globals.h"

// Function declarations for external functions defined in other .cpp files
void updateLights();                            // Declares function from led.cpp to update indicator lights
void checkButtons(unsigned long currentTime);   // Declares function from button.cpp to check button states
void updateTime();                              // Declares function from time.cpp to update the current time
void multiplexDisplay(unsigned long currentTime); // Declares function from display.cpp to handle display multiplexing

// Setup function to initialize hardware pins and serial communication
void setup() {
  // Configure segment pins (a-g) as outputs for the 7-segment display
  for (int i = 0; i < 7; i++) pinMode(segPins[i], OUTPUT);
  // Configure digit selection pins (D1-D4) as outputs for the 7-segment display
  for (int i = 0; i < 4; i++) pinMode(digitPins[i], OUTPUT);
  // Configure the decimal point pin (used as colon) as an output and set it to LOW (off)
  pinMode(dpPin, OUTPUT);
  digitalWrite(dpPin, LOW);

  // Configure indicator light pins (light1, light2, light3) as outputs and set them to LOW (off)
  for (int i = 0; i < 3; i++) {
    pinMode(lightPins[i], OUTPUT);
    digitalWrite(lightPins[i], LOW);
  }

  // Configure button pins as inputs to read button states
  for (int i = 0; i < numButtons; i++) {
    pinMode(buttonPins[i], INPUT);
  }

  // Initialize serial communication at 115200 baud rate for debugging
  Serial.begin(115200);
  Serial.println("Serial Test OK"); // Print a test message to confirm serial setup
}

// Main loop function to continuously run the system logic
void loop() {
  unsigned long currentTime = millis(); // Get the current time in milliseconds since program start
  updateTime();                         // Update the current time (minutes and seconds)
  checkButtons(currentTime);            // Check button states and handle button events
  multiplexDisplay(currentTime);        // Update the 7-segment display using multiplexing
  updateLights();                       // Update the indicator lights based on the current state
}