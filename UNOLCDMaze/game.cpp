#include "game.h"
#include "classes.h"
#include "lcd.h"
#include "buttons.h"
#include "maze.h"
#include "combat.h"
#include "intro.h"
#include "shop.h"

const bool SHOW_CONSOLE_MAP = false;   // prints the full maze to Serial Monitor
const int STATUS_WIDTH = 16;          // status text stays left of the map

static MapSpot maze[MAZE_H][MAZE_W];
static Monster monsters[MAX_MONSTERS];
static int monsterCount = 0;
static Player guy;

static const __FlashStringHelper* statusMsg = nullptr;


// Prints the full maze to the Serial Monitor (replaces mh.drawMaze).
// '@' is the player, monsters show their symbol, everything else uses
// drawSpace().
static void drawMaze(MapSpot maze[MAZE_H][MAZE_W], Monster* monsters, int monsterCount,
              const Player& guy) {
  for (int y = 0; y < MAZE_H; y++) {
    for (int x = 0; x < MAZE_W; x++) {
      char ch;
      Monster* m = monsterAtLocation(monsters, monsterCount, x, y);
      if (y == guy.pos[0] && x == guy.pos[1]) {
        ch = '@';
      } else if (m != nullptr) {
        ch = m->symbol;
      } else {
        ch = drawSpace(maze[y][x].val);
      }
      Serial.print(ch);
    }
    Serial.println();
  }
  Serial.println();
}


// See if the requested player move is valid. Move if it is.
// Returns 'n', 's', 'e', 'w' or 'q'
static char moveGuy(MapSpot maze[MAZE_H][MAZE_W], Player& guy) {
  int buttonPressed = waitPressAndReleaseWithQuit(2);
  char move = ' ';

  if (buttonPressed == buttonLeft) {
    move = 'w';
  } else if (buttonPressed == buttonUp) {
    move = 'n';
  } else if (buttonPressed == buttonDown) {
    move = 's';
  } else if (buttonPressed == buttonRight) {
    move = 'e';
  } else if (buttonPressed == NO_BUTTON) {
    move = 'q';
  }

  int y = guy.pos[0];
  int x = guy.pos[1];

  if (move == 'n') {
    y--;
  } else if (move == 's') {
    y++;
  } else if (move == 'w') {
    x--;
  } else if (move == 'e') {
    x++;
  } else if (move == 'q') {
    guy.gameOver = true;
  }

  if (inbounds(x, y) && maze[y][x].val != WALL) {
    guy.pos[0] = y;
    guy.pos[1] = x;
    maze[y][x].explored = true;
  }

  return move;
}


static void moveMonsters(MapSpot maze[MAZE_H][MAZE_W], Monster* monsters, int monsterCount) {
  for (int i = 0; i < monsterCount; i++) {
    Monster& m = monsters[i];

    if (m.dead) {
      continue;              // skip to the next monster
    }

    if (m.justFought) {      // Monster won't move if just fought
      m.justFought = false;
    } else {
      m.move(maze, monsters, monsterCount);
    }
  }
}


static void movePlayerToEmpty(MapSpot maze[MAZE_H][MAZE_W], Monster* monsters, int monsterCount,
                       Player& guy) {
  Point options[4];
  int count = 0;

  // east, west, south, north
  const int dx[4] = { 1, -1, 0,  0 };
  const int dy[4] = { 0,  0, 1, -1 };

  for (int i = 0; i < 4; i++) {
    int x = guy.pos[1] + dx[i];
    int y = guy.pos[0] + dy[i];
    if (spotEmpty(maze, monsters, monsterCount, x, y)) {
      options[count].y = y;
      options[count].x = x;
      count++;
    }
  }

  if (count > 0) {
    Point newSpot = options[random(count)];
    guy.pos[0] = newSpot.y;
    guy.pos[1] = newSpot.x;
  }
}


static void checkForMonsters(MapSpot maze[MAZE_H][MAZE_W], Monster* monsters, int monsterCount,
                      Player& guy) {
  Monster* monster = monsterAtLocation(monsters, monsterCount, guy.pos[1], guy.pos[0]);

  if (monster != nullptr && !monster->justFought) {
    bool ranAway = combat(*monster, guy, buttonList);
    if (ranAway) {
      movePlayerToEmpty(maze, monsters, monsterCount, guy);
    }
  }
}


// Check the player's current spot.
// Set some status variables if standing on key/trap/exit.
static void checkMoveResult(MapSpot maze[MAZE_H][MAZE_W], Player& guy) {
  uint8_t val = maze[guy.pos[0]][guy.pos[1]].val;
  statusMsg = F("");

  if (val == TRAP) {
    if (guy.trap() > 0) {
      statusMsg = F("A TRAP! OUCH!");
    } else {
      statusMsg = F("Trap dodged!");
    }
  } else if (val == KEY) {
    statusMsg = F("Found a KEY!");
    maze[guy.pos[0]][guy.pos[1]].val = FLOOR;   // Remove key from map
    guy.key = true;
  } else if (val == SHOP) {
    shop(guy, buttonList);
  } else if (val == EXIT) {
    if (guy.key) {
      guy.escaped = true;
    } else {
      statusMsg = F("Exit, no key!");
    }
  }
}


// Not called anywhere in mz.py, but converted in case you use it.
// Fills `text` (needs room for at least 8 chars) with e.g. "N S E".
static void moveDirsGet(MapSpot maze[MAZE_H][MAZE_W], const Player& guy, char* text) {
  int y = guy.pos[0];
  int x = guy.pos[1];
  text[0] = '\0';                      // start with an empty string

  if (inbounds(x, y - 1) && maze[y - 1][x].val != WALL) strcat(text, "N ");
  if (inbounds(x, y + 1) && maze[y + 1][x].val != WALL) strcat(text, "S ");
  if (inbounds(x + 1, y) && maze[y][x + 1].val != WALL) strcat(text, "E ");
  if (inbounds(x - 1, y) && maze[y][x - 1].val != WALL) strcat(text, "W ");

  // Python's .strip(): remove the trailing space, if any
  int len = strlen(text);
  if (len > 0) {
    text[len - 1] = '\0';
  }
}


// The 16-column status area to the left of the map
static void showStatus() {
  char buf[LCD_COLS + 1];

  snprintf_P(buf, sizeof(buf), PSTR("{%d} HP:%d DMG:%d"),
             guy.dungeon, guy.health, guy.damage);
  printLineWidth(0, buf, LEFT, STATUS_WIDTH);

  snprintf_P(buf, sizeof(buf), PSTR("ATK:%d DODGE:%d"), guy.attack, guy.dodge);
  printLineWidth(1, buf, LEFT, STATUS_WIDTH);

  // condition ? a : b picks " KEY" or "" in one line
  snprintf_P(buf, sizeof(buf), PSTR("GOLD:%d%s"), guy.gold, guy.key ? " KEY" : "");
  printLineWidth(2, buf, LEFT, STATUS_WIDTH);

  printLineWidth(3, statusMsg, LEFT, STATUS_WIDTH);
}


void playGame() {
  statusMsg = F("");
  lcd.clear();

  guy = setupPlayer(buttonList);
  char move = ' ';   // declared out here because it's checked after the loops

  while (!guy.gameOver) {
    initMaze(maze, guy);
    monsterCount = monstersAdd(maze, monsters, guy);

    // Need these for how the map is drawn to LCD
    char lastNSMove = 'n';
    char lastEWMove = 'e';

    while (!guy.gameOver && !guy.escaped) {
      showStatus();

      // Show current map
      mapAddToLCD(maze, monsters, monsterCount, guy, lastNSMove, lastEWMove);
      if (SHOW_CONSOLE_MAP) {
        drawMaze(maze, monsters, monsterCount, guy);
      }

      move = moveGuy(maze, guy);

      // We track the moves to help with LCD map positioning
      if (move == 'e') {
        lastEWMove = 'e';
      } else if (move == 'w') {
        lastEWMove = 'w';
      } else if (move == 'n') {
        lastNSMove = 'n';
      } else {
        lastNSMove = 's';
      }

      if (move != 'q') {
        checkForMonsters(maze, monsters, monsterCount, guy);    // after guy moves
        if (!guy.gameOver) {
          moveMonsters(maze, monsters, monsterCount);
          checkForMonsters(maze, monsters, monsterCount, guy);  // after monsters move
        }
        if (!guy.gameOver) {
          checkMoveResult(maze, guy);                           // traps, etc.
        }
      }
    }

    if (guy.escaped && guy.dungeon > 0) {
      printLine(0, F("You escaped to the"), LEFT);
      printLine(1, F("next level!!!"), LEFT);
      if (guy.nextDungeon()) {
        printLine(2, F("You rest a minute"), LEFT);
        printLine(3, F("and heal slightly."), LEFT);
      } else {
        printLine(2, F(""), LEFT);
        printLine(3, F(""), LEFT);
      }
      waitPressAndRelease();
      statusMsg = F("");     // replaces lcdTextClear()
      lcd.clear();
    }
  }

  if (guy.dungeon == 0) {
    printLine(0, F("You have made it out"), LEFT);
    printLine(1, F("to the surface! YES!"), LEFT);
    printLine(2, F("The Necromancer"), LEFT);
    printLine(3, F("appears nearby."), LEFT);
    waitPressAndRelease();

    printLine(0, F("'I see you have"), LEFT);
    printLine(1, F("passed my test', he"), LEFT);
    printLine(2, F("says as he smiles."), LEFT);
    printLine(3, F("You are confused."), LEFT);
    waitPressAndRelease();

    printLine(0, F("As he starts to cast"), LEFT);
    printLine(1, F("a spell, he says"), LEFT);
    printLine(2, F("'It is time for your"), LEFT);
    printLine(3, F("next adventure!'"), LEFT);
    waitPressAndRelease();

    printLine(0, F("To be continued in"), CENTER);
    printLine(1, F("Maze of the"), CENTER);
    printLine(2, F("Necromancer II"), CENTER);
    printLine(3, F(""), CENTER);
    waitPressAndRelease();

    printLine(0, F("Programming by"), CENTER);
    printLine(1, F("Mark Wiedmann"), CENTER);
    printLine(2, F(""), CENTER);
    printLine(3, F("Thanks for playing!"), CENTER);
    waitPressAndRelease();
  } else if (move != 'q') {
    printLine(0, F("As you take your"), LEFT);
    printLine(1, F("final breath,"), LEFT);
    printLine(2, F("the Necromancer's"), LEFT);
    printLine(3, F("voice echoes..."), LEFT);
    waitPressAndRelease();

    printLine(0, F("'Hmm, such a shame."), LEFT);
    printLine(1, F("I had very high"), LEFT);
    printLine(2, F("expectations for"), LEFT);
    printLine(3, F("you.'"), LEFT);
    waitPressAndRelease();
  }
}


void gameSetup() {
  if (SHOW_CONSOLE_MAP) {
    Serial.begin(115200);
  }
  lcdSetup();
  buttonSetup();
  randomSeed(analogRead(A0));   // A0 must be unconnected
}
