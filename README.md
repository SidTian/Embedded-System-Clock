# Embedded-System-Clock
## Overview
The Embedded-System-Clock is a modular embedded system project designed for an Arduino-compatible microcontroller. It implements a digital clock with a 7-segment display, featuring time display, alarm setting, and visual feedback via indicator lights. The system uses buttons for user interaction and employs a state machine to manage its operation modes.

## Features
Time Display: Shows the current time in MM:SS format on a 4-digit 7-segment display with a blinking colon.
Alarm Functionality: Allows setting an alarm time (minutes and seconds) and triggers visual alerts when the alarm time matches the current time.
Button Controls:
Button 1: Switch between displaying time and setting the alarm; long press to exit alarm setting.
Button 2: Increment alarm value (minutes or seconds); short press to stop alarm; used in combination for state transitions.
Button 3: Decrement alarm value (minutes or seconds); used in combination for state transitions.
Indicator Lights:
Green Light (light1): On when displaying current time.
Red Light (light2): On when setting alarm or when alarm is triggered.
Alarm Light (light3): On when alarm minutes match current minutes or when alarm is triggered.
Multiplexed Display: Efficiently drives the 7-segment display using multiplexing to reduce pin usage.
Debounced Buttons: Includes debouncing logic to ensure reliable button input detection.
Hardware Requirements
Microcontroller: Arduino-compatible board (e.g., Arduino Uno, ESP32).
7-Segment Display: 4-digit common cathode 7-segment display with decimal point (for colon).
Buttons: 3 push buttons connected to GPIO pins with pull-down resistors (if not using internal pull-ups).
LEDs: 3 indicator LEDs (green, red, and one for alarm) with current-limiting resistors.
GPIO Pins:
7 pins for segments (a-g).
4 pins for digit selection (D1-D4).
1 pin for decimal point (dp).
3 pins for LEDs.
3 pins for buttons.
Pin Assignments (Default)
Segments (a-g): Pins 13, 14, 15, 18, 19, 21, 22
Digits (D1-D4): Pins 23, 16, 17, 25
Decimal Point (dp): Pin 26
Lights (green, red, alarm): Pins 2, 4, 5
Buttons (1, 2, 3): Pins 27, 32, 33
## Project Structure
The project is modularized into multiple .cpp files with a single header file for global variables.

globals.h: Declares global variables, pin assignments, and the State enumeration.
globals.cpp: Defines global variables and constants (e.g., pin arrays, segment patterns).
main.cpp: Contains setup() and loop() functions, initializing hardware and running the main logic.
led.cpp: Manages the indicator lights based on the system state.
button.cpp: Handles button input detection, debouncing, and state machine logic.
time.cpp: Updates the current time based on millisecond intervals.
display.cpp: Controls the 7-segment display, including multiplexing and digit rendering.
Installation
Clone or Download: Copy the project files into a single directory (e.g., Embedded-System-Clock).
Open in Arduino IDE:
Launch the Arduino IDE.
Open main.cpp from the project directory.
Ensure all other .cpp and .h files are in the same directory (the IDE will automatically include them).
Install Dependencies: No external libraries are required beyond the Arduino core (Arduino.h).
Connect Hardware: Wire the 7-segment display, buttons, and LEDs to the microcontroller according to the pin assignments in globals.cpp.
Upload Code: Select your board and port in the Arduino IDE, then upload the code.
Usage
Power On: The system starts in DISPLAY_TIME mode, showing the current time (default: 14:23).
Set Alarm:
Press Button 1 briefly to enter SET_ALARM_MINUTE mode (red light on).
Use Button 2 to increment minutes, Button 3 to decrement minutes.
Long press Buttons 2 and 3 together to switch to SET_ALARM_SECOND mode.
Adjust seconds similarly with Buttons 2 and 3.
Long press Button 1 to return to DISPLAY_TIME mode.
Alarm Trigger: When the current time matches the alarm time, the system enters ALARM_TRIGGERED mode (red and alarm lights on, colon blinks).
Press Button 2 briefly or long press all three buttons to stop the alarm and return to DISPLAY_TIME.
Visual Feedback:
Green light indicates normal time display.
Red light indicates alarm setting or triggering.
Alarm light blinks or stays on based on alarm conditions.
Customization
Pin Assignments: Modify the pin arrays in globals.cpp to match your hardware setup.
Timing: Adjust debounceDelay, blinkInterval, or longPressDelay in globals.cpp for different timing behaviors.
Initial Time: Change currentMinutes and currentSeconds in globals.cpp to set a different starting time.
## License
This project is open-source and available under the . Feel free to modify and distribute it as needed.

## Contributing
Contributions are welcome! Please submit a pull request or open an issue for bug reports, feature requests, or improvements.

## Notes
The README is written in a standard format for GitHub or similar platforms, with clear sections for easy navigation.
It includes practical details like pin assignments and usage instructions, making it accessible to users unfamiliar with the code.
You can save this as README.md in your project directory and adjust any specifics (e.g., license, pin numbers) as needed.