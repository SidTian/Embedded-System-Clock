#include "globals.h"

// Function to update the current time (minutes and seconds) based on elapsed milliseconds
void updateTime() {
  unsigned long currentTime = millis(); // Get the current time in milliseconds since program start
  // Check if at least 1000ms (1 second) has passed since the last update
  if (currentTime - lastTimeUpdate >= 1000) {
    currentSeconds++; // Increment the seconds counter
    // If seconds reach 60, reset to 0 and increment minutes
    if (currentSeconds >= 60) {
      currentSeconds = 0;
      currentMinutes++; // Increment the minutes counter
      // If minutes reach 60, reset to 0 (hours are not tracked in this system)
      if (currentMinutes >= 60) currentMinutes = 0;
    }
    lastTimeUpdate = currentTime; // Update the timestamp of the last time update
  }
}