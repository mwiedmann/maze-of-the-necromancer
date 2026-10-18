#include "combat.h"
#include "lcd.h"
#include "buttons.h"

// Copies a flash-stored name (like monster.name) into a normal char
// buffer so it can be used with snprintf. maxLen limits how many
// characters are copied.
static void copyName(char* dest, const __FlashStringHelper* name, int maxLen) {
  strncpy_P(dest, (const char*)name, maxLen);
  dest[maxLen] = '\0';   // strncpy doesn't always add the end marker
}

// Monster& and Player& are references, so changes to health, gold,
// dead, etc. affect the real objects
bool combat(Monster& monster, Player& guy, const int buttonList[]) {
  // buf holds one formatted line: 20 characters + '\0'
  char buf[LCD_COLS + 1];
  char name[11];         // longest name is 10 chars
  copyName(name, monster.name, 10);

  printLine(0, F("********************"), LEFT);
  printLine(1, F("You are attacked by"), CENTER);

  snprintf_P(buf, sizeof(buf), PSTR("a %s"), name);
  printLine(2, buf, CENTER);
  printLine(3, F("********************"), LEFT);
  waitPressAndRelease();

  bool ranAway = false;
  monster.justFought = true;

  while (!monster.dead && guy.health > 0 && !ranAway) {
    char shortName[10];
    copyName(shortName, monster.name, 9);
    snprintf_P(buf, sizeof(buf), PSTR("%s HP:%d DMG:%d"),
               shortName, monster.health, monster.damage);
    printLine(0, buf, LEFT);

    snprintf_P(buf, sizeof(buf), PSTR("You HP:%d DMG:%d"),
               guy.health, guy.damage);
    printLine(1, buf, LEFT);

    printLine(2, F("BLUE: Atk  RED: Def"), LEFT);
    printLine(3, F("YELLOW: Run"), LEFT);
    int buttonPressed = waitPressAndRelease();

    int attack = guy.attack;
    int dodge = guy.dodge;
    bool triedToRun = false;

    printLine(0, F("You decide to try"), LEFT);

    if (buttonPressed == buttonList[0]) {          // BLUE: Attack
      attack += 15;
      printLine(1, F("an all out attack,"), LEFT);
    } else if (buttonPressed == buttonList[3]) {   // RED: Defense
      dodge += 15;
      printLine(1, F("a defensive strike,"), LEFT);
    } else {                                       // Run
      printLine(1, F("to bravely run away,"), LEFT);
      triedToRun = true;
      ranAway = guy.skillCheck(guy.run);
    }

    if (triedToRun) {
      if (ranAway) {
        printLine(2, F("and you make your"), LEFT);
        printLine(3, F("escape!"), LEFT);
      } else {
        printLine(2, F("but the creature"), LEFT);
        printLine(3, F("stops and wounds you"), LEFT);
        guy.takeDamage(monster.damage);
      }
    } else {
      if (guy.skillCheck(attack)) {
        monster.takeDamage(guy.damage);
        printLine(2, F("and you hit!"), LEFT);
      } else {
        printLine(2, F("and you miss."), LEFT);
      }

      if (guy.skillCheck(dodge)) {
        printLine(3, F("You swiftly dodge."), LEFT);
      } else {
        snprintf_P(buf, sizeof(buf), PSTR("You take %d damage."), monster.damage);
        printLine(3, buf, LEFT);
        guy.takeDamage(monster.damage);
      }
    }

    waitPressAndRelease();
  }

  if (monster.dead && guy.health > 0) {
    printLine(0, F(""), CENTER);
    snprintf_P(buf, sizeof(buf), PSTR("%s defeated!"), name);
    printLine(1, buf, CENTER);
    snprintf_P(buf, sizeof(buf), PSTR("You find %d gold"), monster.gold);
    printLine(2, buf, CENTER);
    printLine(3, F(""), CENTER);

    guy.gold += monster.gold;   // Guy gets the gold
    waitPressAndRelease();
  }

  return ranAway;
}
