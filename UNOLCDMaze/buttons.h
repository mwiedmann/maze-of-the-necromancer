#pragma once
#include <Arduino.h>

const int buttonLeft  = 10;  // Blue
const int buttonUp    = 9;   // Yellow
const int buttonDown  = 8;   // Green
const int buttonRight = 7;   // Red

const int NO_BUTTON = 255;

extern const int buttonList[4];   // defined in buttons.cpp

void buttonSetup();
int waitPressAndRelease();
int waitPressAndReleaseWithQuit(int timeout);
