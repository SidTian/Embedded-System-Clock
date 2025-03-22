#include "globals.h"

// Function to update the state of the indicator lights based on the current system state
void updateLights() {
  switch (currentState) { // Switch based on the current state of the system
    case DISPLAY_TIME: // State: Displaying the current time
      digitalWrite(lightPins[0], HIGH); // Turn on green light (light1)
      digitalWrite(lightPins[1], LOW);  // Turn off red light (light2)
      // Turn on light3 if current minutes match alarm minutes, otherwise turn it off
      digitalWrite(lightPins[2], (currentMinutes == alarmMinutes) ? HIGH : LOW);
      break;

    case SET_ALARM_MINUTE: // State: Setting alarm minutes
    case SET_ALARM_SECOND: // State: Setting alarm seconds
      digitalWrite(lightPins[0], LOW);  // Turn off green light (light1)
      digitalWrite(lightPins[1], HIGH); // Turn on red light (light2)
      digitalWrite(lightPins[2], LOW);  // Turn off alarm light (light3)
      break;

    case ALARM_TRIGGERED: // State: Alarm is triggered
      digitalWrite(lightPins[0], LOW);  // Turn off green light (light1)
      digitalWrite(lightPins[1], HIGH); // Turn on red light (light2) continuously
      digitalWrite(lightPins[2], HIGH); // Turn on alarm light (light3)
      break;
  }
}