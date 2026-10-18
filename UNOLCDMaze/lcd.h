#pragma once
#include <Arduino.h>
#include <LiquidCrystal.h>

const int LCD_COLS = 20;
const int LCD_ROWS = 4;

const int LEFT   = 1;
const int CENTER = 2;
const int RIGHT  = 3;

extern LiquidCrystal lcd;   // defined in lcd.cpp

void lcdSetup();
void printLineWidth(int row, const char* text, int style, int width);
void printLineWidth(int row, const __FlashStringHelper* text, int style, int width);
void printLine(int row, const char* text, int style);
void printLine(int row, const __FlashStringHelper* text, int style);
