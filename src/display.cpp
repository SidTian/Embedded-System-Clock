#include "globals.h"

// Function to clear all segment pins by setting them to LOW (off state for common cathode display)
void clear() {
  for (int i = 0; i < 7; i++) digitalWrite(segPins[i], LOW); // Turn off all 7 segments (a-g)
}

// Function to display a single number on the 7-segment display
void displayNumber(int num) {
  clear(); // Clear the display before showing a new number
  unsigned char pattern = numberPatterns[num]; // Get the segment pattern for the given number (0-9)
  // Loop through each segment (a-g) and set it HIGH or LOW based on the pattern
  for (int i = 0; i < 7; i++) digitalWrite(segPins[i], (pattern >> (6 - i)) & 0x01); // Shift and mask to get each bit
}

// Function to display a digit at a specific position (D1-D4)
void displayDigit(int digit, int position) {
  // Disable all digit pins by setting them HIGH (common cathode, HIGH = off)
  for (int i = 0; i < 4; i++) digitalWrite(digitPins[i], HIGH);
  digitalWrite(dpPin, LOW); // Turn off the decimal point (dp) pin
  displayNumber(digit); // Display the specified digit on the segments
  digitalWrite(digitPins[position], LOW); // Enable the specific digit position (LOW = on for common cathode)
}

// Function to display the colon (time separator) between minutes and seconds
void displayColon() {
  // Disable all digit pins by setting them HIGH
  for (int i = 0; i < 4; i++) digitalWrite(digitPins[i], HIGH);
  clear(); // Clear all segments
  digitalWrite(digitPins[1], LOW); // Enable digit 2 (D2) to show the colon
  digitalWrite(dpPin, HIGH); // Turn on the decimal point pin to display the colon
}

// Function to multiplex the display, showing digits and colon in sequence
void multiplexDisplay(unsigned long currentTime) {
  // Check if enough time has passed (3ms) to switch to the next display position
  if (currentTime - lastScan >= 3) {
    int minuteTens, minuteUnits, secondTens, secondUnits; // Variables to hold the digits to display

    // Determine which time to display based on the current state
    if (currentState == DISPLAY_TIME || currentState == ALARM_TRIGGERED) {
      minuteTens = currentMinutes / 10;   // Tens digit of current minutes
      minuteUnits = currentMinutes % 10;  // Units digit of current minutes
      secondTens = currentSeconds / 10;   // Tens digit of current seconds
      secondUnits = currentSeconds % 10;  // Units digit of current seconds
    } else { // SET_ALARM_MINUTE or SET_ALARM_SECOND state
      minuteTens = alarmMinutes / 10;     // Tens digit of alarm minutes
      minuteUnits = alarmMinutes % 10;    // Units digit of alarm minutes
      secondTens = alarmSeconds / 10;     // Tens digit of alarm seconds
      secondUnits = alarmSeconds % 10;    // Units digit of alarm seconds
    }

    // Handle blinking effect by toggling blinkState every blinkInterval (500ms)
    if (currentTime - lastBlink >= blinkInterval) {
      blinkState = !blinkState; // Toggle between showing and hiding the display
      lastBlink = currentTime;  // Update the last blink timestamp
    }

    // Display the appropriate digit or colon based on the current scan position
    switch (scanPosition) {
      case 0: // Display minute tens digit (D1)
        if (currentState == SET_ALARM_MINUTE && !blinkState) clear(); // Blink off during alarm minute setting
        else displayDigit(minuteTens, 0); // Show minute tens digit
        break;
      case 1: // Display minute units digit (D2)
        if (currentState == SET_ALARM_MINUTE && !blinkState) clear(); // Blink off during alarm minute setting
        else displayDigit(minuteUnits, 1); // Show minute units digit
        break;
      case 2: // Display second tens digit (D3)
        if (currentState == SET_ALARM_SECOND && !blinkState) clear(); // Blink off during alarm second setting
        else displayDigit(secondTens, 2); // Show second tens digit
        break;
      case 3: // Display second units digit (D4)
        if (currentState == SET_ALARM_SECOND && !blinkState) clear(); // Blink off during alarm second setting
        else displayDigit(secondUnits, 3); // Show second units digit
        break;
      case 4: // Display the colon
        if (currentState == ALARM_TRIGGERED && !blinkState) clear(); // Blink off when alarm is triggered
        else displayColon(); // Show the colon
        break;
    }

    scanPosition++; // Move to the next position in the multiplexing sequence
    if (scanPosition >= 5) scanPosition = 0; // Reset to 0 after reaching the last position (colon)
    lastScan = currentTime; // Update the last scan timestamp
  }
}