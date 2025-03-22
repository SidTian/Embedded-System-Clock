#include "globals.h"

// Define segment pins (a-g) using safe GPIO pins for a 7-segment display
const int segPins[7] = {13, 14, 15, 18, 19, 21, 22}; // Pins for segments: a, b, c, d, e, f, g
const int digitPins[4] = {23, 16, 17, 25};           // Pins for digit selection: D1, D2, D3, D4
const int dpPin = 26;                                // Pin for the decimal point (used as colon in clock)
const int lightPins[3] = {2, 4, 5};                  // Pins for indicator lights: light1 (green), light2 (red), light3 (alarm)
const int buttonPins[3] = {27, 32, 33};              // Pins for buttons: button1, button2, button3

// Define the segment patterns for displaying numbers 0-9 on a 7-segment display (bitmaps)
// Each bit corresponds to a segment (a-g), where 1 = on and 0 = off
const unsigned char numberPatterns[10] = {
    0b1111110, // 0: Segments a, b, c, d, e, f (all except g)
    0b0110000, // 1: Segments b, c
    0b1101101, // 2: Segments a, b, d, e, g
    0b1111001, // 3: Segments a, b, c, d, g
    0b0110011, // 4: Segments b, c, f, g
    0b1011011, // 5: Segments a, c, d, f, g
    0b1011111, // 6: Segments a, c, d, e, f, g
    0b1110000, // 7: Segments a, b, c
    0b1111111, // 8: Segments a, b, c, d, e, f, g (all on)
    0b1111011  // 9: Segments a, b, c, f, g
};

// Button-related global variables
const int numButtons = 3;                              // Number of buttons in the system
int buttonStates[3] = {0, 0, 0};                       // Array to store current button states (0 = released, 1 = pressed)
int lastButtonStates[3] = {1, 1, 1};                   // Array to store previous button states (initially released)
unsigned long lastButtonPressTimes[3] = {0, 0, 0};     // Array to store timestamps of the last button state change
bool buttonProcessed[3] = {false, false, false};       // Array to track whether each button press has been processed
const int debounceDelay = 50;                          // Debounce delay in milliseconds to filter button noise

// Global variables for timekeeping and state management
State currentState = DISPLAY_TIME;        // Current state of the system (starts in DISPLAY_TIME mode)
int currentMinutes = 14;                  // Current time: minutes (initialized to 14)
int currentSeconds = 23;                  // Current time: seconds (initialized to 23)
int alarmMinutes = 8;                     // Alarm time: minutes (initialized to 8)
int alarmSeconds = 0;                     // Alarm time: seconds (initialized to 0)
unsigned long lastTimeUpdate = 0;         // Timestamp of the last time update (in milliseconds)
unsigned long lastScan = 0;               // Timestamp of the last display scan (in milliseconds)
int scanPosition = 0;                     // Current position in the multiplexing sequence (0-4)
unsigned long lastBlink = 0;              // Timestamp of the last blink toggle (in milliseconds)
bool blinkState = false;                  // Blink state (true = display on, false = display off)
const int blinkInterval = 500;            // Blink interval in milliseconds (500ms = 0.5s)
const int longPressDelay = 1000;          // Long press delay in milliseconds (1000ms = 1s)