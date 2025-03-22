#include "globals.h"

// Function declarations
void checkButtons(unsigned long currentTime); // Check the state of all buttons and handle debouncing
void handleStateMachine(unsigned long currentTime, bool button1Held, bool button23Held, bool button123Held); // Handle state transitions based on button inputs

// Function to check button states and detect presses, releases, and long presses
void checkButtons(unsigned long currentTime) {
  // Loop through all buttons
  for (int i = 0; i < numButtons; i++) {
    // Read the analog voltage from the button pin
    int buttonVoltage = analogRead(buttonPins[i]);
    // Determine the current button state (0 = pressed, 1 = released) based on voltage threshold
    int currentButtonState = (buttonVoltage < 500) ? 0 : 1;

    // Check if the button state has changed since the last reading
    if (currentButtonState != lastButtonStates[i]) {
      // Apply debounce delay to filter out noise
      if (currentTime - lastButtonPressTimes[i] > debounceDelay) {
        if (currentButtonState == 0) { // Button is pressed
          buttonStates[i] = 1;         // Update current state to pressed
          buttonProcessed[i] = false;  // Reset processed flag for this button
          Serial.print("Button ");     // Log the button press event
          Serial.print(i + 1);
          Serial.println(" pressed!");
        } else { // Button is released
          buttonStates[i] = 0;       // Update current state to released
          Serial.print("Button ");   // Log the button release event
          Serial.print(i + 1);
          Serial.println(" released!");
        }
        // Record the time of this state change
        lastButtonPressTimes[i] = currentTime;
      }
      // Update the last known state of the button
      lastButtonStates[i] = currentButtonState;
    }
  }

  // Check if Button 1 is held for a long press
  bool button1Held = (buttonStates[0] == 1 && (currentTime - lastButtonPressTimes[0] > longPressDelay));

  // Variables to track simultaneous long press of Buttons 2 and 3
  static unsigned long button23HoldStart = 0; // Time when Buttons 2 and 3 were both pressed
  static bool button23Processed = false;      // Flag to track if the long press has been processed
  bool button23Held = false;                  // Indicates if Buttons 2 and 3 are held long enough

  // Detect if Buttons 2 and 3 are pressed simultaneously
  if (buttonStates[1] == 1 && buttonStates[2] == 1) {
    if (button23HoldStart == 0) button23HoldStart = currentTime; // Record start time of simultaneous press
    // Check if the long press duration exceeds the threshold and hasn't been processed yet
    if (currentTime - button23HoldStart > longPressDelay && !button23Processed) button23Held = true;
  } else {
    button23HoldStart = 0;   // Reset start time if both buttons aren't pressed
    button23Processed = false; // Reset processed flag
  }

  // Variables to track simultaneous long press of Buttons 1, 2, and 3
  static unsigned long button123HoldStart = 0; // Time when all three buttons were pressed
  static bool button123Processed = false;      // Flag to track if the long press has been processed
  bool button123Held = false;                  // Indicates if all three buttons are held long enough

  // Detect if Buttons 1, 2, and 3 are pressed simultaneously
  if (buttonStates[0] == 1 && buttonStates[1] == 1 && buttonStates[2] == 1) {
    if (button123HoldStart == 0) button123HoldStart = currentTime; // Record start time of simultaneous press
    // Check if the long press duration exceeds the threshold and hasn't been processed yet
    if (currentTime - button123HoldStart > longPressDelay && !button123Processed) button123Held = true;
  } else {
    button123HoldStart = 0;   // Reset start time if all buttons aren't pressed
    button123Processed = false; // Reset processed flag
  }

  // Mark Button 1 as processed if it was held long enough
  buttonProcessed[0] |= button1Held;
  // Mark Buttons 2 and 3 long press as processed
  if (button23Held) button23Processed = true;
  // Mark all three buttons long press as processed
  if (button123Held) button123Processed = true;

  // Call the state machine with the current button states
  handleStateMachine(currentTime, button1Held, button23Held, button123Held);
}

// Function to handle the state machine logic based on button inputs
void handleStateMachine(unsigned long currentTime, bool button1Held, bool button23Held, bool button123Held) {
  switch (currentState) {
    case DISPLAY_TIME: // State: Displaying current time
      // Short press of Button 1 switches to setting alarm minutes
      if (buttonStates[0] == 1 && !buttonProcessed[0] && !button1Held) {
        currentState = SET_ALARM_MINUTE;
        buttonProcessed[0] = true;
      }
      // Trigger alarm if current minutes match alarm minutes
      if (currentMinutes == alarmMinutes) {
        currentState = ALARM_TRIGGERED;
      }
      break;

    case SET_ALARM_MINUTE: // State: Setting alarm minutes
      if (button1Held) { // Long press of Button 1 returns to displaying time
        currentState = DISPLAY_TIME;
      } else if (buttonStates[1] == 1 && !buttonProcessed[1]) { // Short press of Button 2 increments alarm minutes
        alarmMinutes++;
        if (alarmMinutes >= 60) alarmMinutes = 0; // Wrap around to 0 if exceeds 59
        buttonProcessed[1] = true;
      } else if (buttonStates[2] == 1 && !buttonProcessed[2]) { // Short press of Button 3 decrements alarm minutes
        alarmMinutes--;
        if (alarmMinutes < 0) alarmMinutes = 59; // Wrap around to 59 if below 0
        buttonProcessed[2] = true;
      } else if (button23Held) { // Long press of Buttons 2 and 3 switches to setting alarm seconds
        currentState = SET_ALARM_SECOND;
      }
      break;

    case SET_ALARM_SECOND: // State: Setting alarm seconds
      if (button1Held) { // Long press of Button 1 returns to displaying time
        currentState = DISPLAY_TIME;
      } else if (buttonStates[1] == 1 && !buttonProcessed[1]) { // Short press of Button 2 increments alarm seconds
        alarmSeconds++;
        if (alarmSeconds >= 60) alarmSeconds = 0; // Wrap around to 0 if exceeds 59
        buttonProcessed[1] = true;
      } else if (buttonStates[2] == 1 && !buttonProcessed[2]) { // Short press of Button 3 decrements alarm seconds
        alarmSeconds--;
        if (alarmSeconds < 0) alarmSeconds = 59; // Wrap around to 59 if below 0
        buttonProcessed[2] = true;
      } else if (button23Held) { // Long press of Buttons 2 and 3 switches back to setting alarm minutes
        currentState = SET_ALARM_MINUTE;
      }
      break;

    case ALARM_TRIGGERED: // State: Alarm is triggered
      if (buttonStates[1] == 1 && !buttonProcessed[1]) { // Short press of Button 2 stops the alarm
        currentState = DISPLAY_TIME;
        buttonProcessed[1] = true;
      } else if (button123Held) { // Long press of all three buttons stops the alarm
        currentState = DISPLAY_TIME;
      }
      break;
  }
}