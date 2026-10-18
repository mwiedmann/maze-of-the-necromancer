#include "shop.h"
#include "lcd.h"
#include "buttons.h"

static void notEnoughGold() {
  printLine(0, F(""), CENTER);
  printLine(1, F("Hmm, you don't have"), CENTER);
  printLine(2, F("enough gold for that"), CENTER);
  printLine(3, F(""), LEFT);
  waitPressAndRelease();
}

static int guyBuy(Player& guy, const int cost[], int costCount, const int buttonList[]) {
  const int noBuyVal = 3;
  int buttonPressed = waitPressAndRelease();

  if (buttonPressed == buttonList[3]) {
    return noBuyVal;
  }

  for (int i = 0; i < costCount; i++) {
    if (buttonPressed == buttonList[i]) {
      if (guy.buy(cost[i])) {
        return i;
      } else {
        notEnoughGold();
        return noBuyVal;
      }
    }
  }

  return noBuyVal;
}

void shop(Player& guy, const int buttonList[]) {
  const int quitSelection = 3;
  char buf[LCD_COLS + 1];   // for lines with numbers in them

  printLine(0, F("You approach an old"), LEFT);
  printLine(1, F("woman. She smiles at"), LEFT);
  printLine(2, F("you and says, 'I'll"), LEFT);
  printLine(3, F("help...for a price'"), LEFT);
  waitPressAndRelease();

  while (true) {
    printLine(0, F("BLUE: Healing"), LEFT);
    printLine(1, F("YELLOW: Attack"), LEFT);
    printLine(2, F("GREEN: Dodge"), LEFT);
    snprintf_P(buf, sizeof(buf), PSTR("RED: BACK  Gold:%d"), guy.gold);
    printLine(3, buf, LEFT);
    int buttonPressed = waitPressAndRelease();

    if (buttonPressed == buttonList[0]) {           // Healing
      const int cost[3] = { 2, 3, 5 };
      const int vals[3] = { 2, 4, 8 };
      int selection = 0;

      while (selection != quitSelection) {
        snprintf_P(buf, sizeof(buf), PSTR("BLUE: Heal 2   [%dg]"), cost[0]);
        printLine(0, buf, LEFT);
        snprintf_P(buf, sizeof(buf), PSTR(" YLW: Heal 4   [%dg]"), cost[1]);
        printLine(1, buf, LEFT);
        snprintf_P(buf, sizeof(buf), PSTR("GREN: Heal 8   [%dg]"), cost[2]);
        printLine(2, buf, LEFT);
        snprintf_P(buf, sizeof(buf), PSTR("Gold:%d HP:%d/%d"),
                   guy.gold, guy.health, guy.startHealth);
        printLine(3, buf, LEFT);

        selection = guyBuy(guy, cost, 3, buttonList);

        if (selection < 3) {
          guy.heal(vals[selection]);
        }
      }

    } else if (buttonPressed == buttonList[1]) {    // Attack
      const int cost[3] = { 2, 5, 8 };
      const int vals[3] = { 1, 3, 5 };
      int selection = 0;

      while (selection != quitSelection) {
        snprintf_P(buf, sizeof(buf), PSTR("BLUE: Attack +1 [%dg]"), cost[0]);
        printLine(0, buf, LEFT);
        snprintf_P(buf, sizeof(buf), PSTR(" YLW: Attack +3 [%dg]"), cost[1]);
        printLine(1, buf, LEFT);
        snprintf_P(buf, sizeof(buf), PSTR("GREN: Attack +5 [%dg]"), cost[2]);
        printLine(2, buf, LEFT);
        snprintf_P(buf, sizeof(buf), PSTR("Gold:%d Attack:%d"), guy.gold, guy.attack);
        printLine(3, buf, LEFT);

        selection = guyBuy(guy, cost, 3, buttonList);

        if (selection < 3) {
          guy.upgradeAttack(vals[selection]);
        }
      }

    } else if (buttonPressed == buttonList[2]) {    // Dodge
      const int cost[3] = { 2, 5, 8 };
      const int vals[3] = { 1, 3, 5 };
      int selection = 0;

      while (selection != quitSelection) {
        snprintf_P(buf, sizeof(buf), PSTR("BLUE: Dodge +1 [%dg]"), cost[0]);
        printLine(0, buf, LEFT);
        snprintf_P(buf, sizeof(buf), PSTR(" YLW: Dodge +3 [%dg]"), cost[1]);
        printLine(1, buf, LEFT);
        snprintf_P(buf, sizeof(buf), PSTR("GREN: Dodge +5 [%dg]"), cost[2]);
        printLine(2, buf, LEFT);
        snprintf_P(buf, sizeof(buf), PSTR("Gold:%d Dodge:%d"), guy.gold, guy.dodge);
        printLine(3, buf, LEFT);

        selection = guyBuy(guy, cost, 3, buttonList);

        if (selection < 3) {
          guy.upgradeDodge(vals[selection]);
        }
      }

    } else {                                        // RED: leave shop
      return;
    }
  }
}
