#include "buttons.h"

const int buttonList[4] = { buttonLeft, buttonUp, buttonDown, buttonRight };

// Returns the INDEX of the first button found at val, or NO_BUTTON
static int checkButtonList(int val) {
  for (int i = 0; i < 4; i++) {
    if (digitalRead(buttonList[i]) == val) {
      return i;
    }
  }
  return NO_BUTTON;
}

// Waits for a press and returns the button's INDEX
static int waitPress() {
  while (true) {
    int button = checkButtonList(LOW);
    if (button != NO_BUTTON) {
      return button;
    }
  }
}

static void waitRelease(int buttonListIdx) {
  const unsigned long debounceMs = 50;
  unsigned long lastChange = 0;

  while (true) {
    while (digitalRead(buttonList[buttonListIdx]) == LOW) {}

    lastChange = millis();

    while (digitalRead(buttonList[buttonListIdx]) != LOW) {
      if (millis() - lastChange > debounceMs) {
        return;
      }
    }
  }
}

void buttonSetup() {
  for (int i = 0; i < 4; i++) {
    pinMode(buttonList[i], INPUT_PULLUP);
  }
}

// Returns the PIN of the pressed button, to match the game code
int waitPressAndReleaseWithQuit(int timeout) {
  int buttonListIdx = waitPress();
  // Start time
  waitRelease(buttonListIdx);

  // check time and return NO_BUTTON if held for long time (used to quit/restart game)
  return buttonList[buttonListIdx];
}

// Returns the PIN of the pressed button, to match the game code
int waitPressAndRelease() {
  int buttonListIdx = waitPress();
  waitRelease(buttonListIdx);
  return buttonList[buttonListIdx];
}
