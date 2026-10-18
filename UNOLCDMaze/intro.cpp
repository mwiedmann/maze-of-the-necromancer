#include "intro.h"
#include "lcd.h"
#include "buttons.h"

Player setupPlayer(const int buttonList[]) {
  printLine(0, F("Welcome to"), CENTER);
  printLine(1, F("The Maze of the"), CENTER);
  printLine(2, F("Necromancer"), CENTER);
  printLine(3, F(""), LEFT);
  waitPressAndRelease();  // Wait for button

  printLine(0, F("MENU"), CENTER);
  printLine(1, F("--------------------"), CENTER);
  printLine(2, F("BLUE: See intro"), CENTER);
  printLine(3, F("RED: Skip to game"), CENTER);
  int buttonPressed = waitPressAndRelease();

  if (buttonPressed == buttonList[0]) {
    printLine(0, F("You are trapped in "), LEFT);
    printLine(1, F("the depths of the"), LEFT);
    printLine(2, F("evil Necromancer's"), LEFT);
    printLine(3, F("dungeon!"), LEFT);
    waitPressAndRelease();

    printLine(0, F("Can you make your"), LEFT);
    printLine(1, F("way back to the"), LEFT);
    printLine(2, F("surface and gain"), LEFT);
    printLine(3, F("your freedom?"), LEFT);
    waitPressAndRelease();

    printLine(0, F("Find the key and the"), LEFT);
    printLine(1, F("exit on each level."), LEFT);
    printLine(2, F("Beware of monsters"), LEFT);
    printLine(3, F("and traps!"), LEFT);
    waitPressAndRelease();
  }

  // End of intro
  printLine(0, F("Choose your class:"), LEFT);
  printLine(1, F("RED: Rogue"), LEFT);
  printLine(2, F("BLUE: Barbarian"), LEFT);
  printLine(3, F("YELLOW: Mystic"), LEFT);
  buttonPressed = waitPressAndRelease();

  char charClass;
  const char* fullClass;
  const char* abilities;

  if (buttonPressed == buttonList[0]) {
    charClass = 'B';
    fullClass = "Barbarian";
    abilities = "Tough but slow";
  } else if (buttonPressed == buttonList[1]) {
    charClass = 'M';
    fullClass = "Mystic";
    abilities = "Invisibility";
  } else {
    charClass = 'R';
    fullClass = "Rogue";
    abilities = "Evasive";
  }

  printLine(0, F("You have selected"), LEFT);
  printLine(1, fullClass, LEFT);    // no F(): these are variables, so
  printLine(2, F("Ability: "), LEFT);
  printLine(3, abilities, LEFT);    // the regular version is used
  waitPressAndRelease();
  lcd.clear();

  return Player(charClass, fullClass, abilities);
}
