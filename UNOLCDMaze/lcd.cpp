#include "lcd.h"

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);   // the one real definition

// Call once from setup()
void lcdSetup() {
  lcd.begin(LCD_COLS, LCD_ROWS);
  lcd.clear();
}

// Prints text on one row within the first `width` columns.
// Pads with spaces to exactly `width`, so columns beyond it (the map)
// are left alone.
void printLineWidth(int row, const char* text, int style, int width) {
  int len = strlen(text);
  if (len > width) {
    len = width;
  }

  int start = 0;                 // LEFT
  if (style == CENTER) {
    start = (width - len) / 2;
  } else if (style == RIGHT) {
    start = width - len;
  }

  lcd.setCursor(0, row);
  for (int col = 0; col < width; col++) {
    if (col >= start && col < start + len) {
      lcd.write(text[col - start]);
    } else {
      lcd.write(' ');
    }
  }
}

void printLineWidth(int row, const __FlashStringHelper* text, int style, int width) {
  char buf[LCD_COLS + 1];
  strncpy_P(buf, (const char*)text, LCD_COLS);
  buf[LCD_COLS] = '\0';
  printLineWidth(row, buf, style, width);
}

// Full-width versions, used everywhere else. They just pass along 20.
void printLine(int row, const char* text, int style) {
  printLineWidth(row, text, style, LCD_COLS);
}

void printLine(int row, const __FlashStringHelper* text, int style) {
  printLineWidth(row, text, style, LCD_COLS);
}
