#ifndef GLOBALS_H
#define GLOBALS_H
// Header guard to prevent multiple inclusions of this file during compilation

#include <Arduino.h>
// Include the Arduino library for access to standard functions and types (e.g., pinMode, digitalWrite)

// Declare segment pins (a-g) as external constants for a 7-segment display, defined in globals.cpp
extern const int segPins[7];       // Array of pins for segments: a, b, c, d, e, f, g
extern const int digitPins[4];     // Array of pins for digit selection: D1, D2, D3, D4
extern const int dpPin;            // Pin for the decimal point (used as the colon in the clock)
extern const int lightPins[3];     // Array of pins for indicator lights: light1 (green), light2 (red), light3 (alarm)
extern const int buttonPins[3];    // Array of pins for buttons: button1, button2, button3

// Declare the segment patterns for displaying numbers 0-9 as an external constant array
extern const unsigned char numberPatterns[10]; // Bitmap patterns for digits 0-9 on a 7-segment display

// Button-related global variables, declared as external (defined in globals.cpp)
extern const int numButtons;                              // Number of buttons in the system
extern int buttonStates[3];                               // Array to store current button states (0 = released, 1 = pressed)
extern int lastButtonStates[3];                           // Array to store previous button states
extern unsigned long lastButtonPressTimes[3];             // Array to store timestamps of the last button state change
extern bool buttonProcessed[3];                           // Array to track whether each button press has been processed
extern const int debounceDelay;                           // Debounce delay in milliseconds to filter button noise

// Enumeration for the state machine, defining the possible states of the system
enum State {
  DISPLAY_TIME,       // State for displaying the current time
  SET_ALARM_MINUTE,   // State for setting the alarm minutes
  SET_ALARM_SECOND,   // State for setting the alarm seconds
  ALARM_TRIGGERED     // State when the alarm is triggered
};

// Global variables for timekeeping and state management, declared as external (defined in globals.cpp)
extern State currentState;           // Current state of the system
extern int currentMinutes;           // Current time: minutes
extern int currentSeconds;           // Current time: seconds
extern int alarmMinutes;             // Alarm time: minutes
extern int alarmSeconds;             // Alarm time: seconds
extern unsigned long lastTimeUpdate; // Timestamp of the last time update (in milliseconds)
extern unsigned long lastScan;       // Timestamp of the last display scan (in milliseconds)
extern int scanPosition;             // Current position in the multiplexing sequence (0-4)
extern unsigned long lastBlink;      // Timestamp of the last blink toggle (in milliseconds)
extern bool blinkState;              // Blink state (true = display on, false = display off)
extern const int blinkInterval;      // Blink interval in milliseconds
extern const int longPressDelay;     // Long press delay in milliseconds

#endif
// End of the header guard