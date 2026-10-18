#include "maze.h"
#include "lcd.h"         // mapAddToLCD() uses lcd

// ===== Monster table =====
// Each monster type is a struct, and the
// tables are marked PROGMEM so they stay in flash instead of using RAM.
// name[11] fits the longest name ("Fire Giant" = 10 chars + '\0').
struct MonsterType {
  char name[11];
  char symbol;
  uint8_t health;
  uint8_t damage;
};

const MonsterType tier1[] PROGMEM = {
  // Name, Symbol, Health, Damage
  { "Orc",      'o', 1, 1 },
  { "Goblin",   'g', 1, 1 },
  { "Slime",    's', 1, 1 },
  { "Zombie",   'z', 1, 1 },
  { "Rat",      'r', 1, 1 },
  { "Skeleton", 's', 1, 1 },
  { "Bat",      'b', 1, 1 },
  { "Swarm",    'i', 1, 1 },
  { "Hag",      'h', 1, 1 },
  { "Worg",     'w', 1, 1 },
  { "Kobold",   'k', 1, 1 }
};

const MonsterType tier2[] PROGMEM = {
  { "Ogre",     'O', 2, 2 },
  { "Minotaur", 'm', 2, 2 },
  { "Imp",      'I', 1, 2 },
  { "Ghoul",    'G', 1, 2 },
  { "Werewolf", 'W', 2, 1 },
  { "Troll",    'T', 2, 2 },
  { "Ooze",     'O', 2, 1 },
  { "Centaur",  'c', 2, 1 },
  { "Yeti",     'Y', 2, 2 }
};

const MonsterType tier3[] PROGMEM = {
  { "Dragon",     'D', 4, 4 },
  { "Chimera",    'C', 3, 2 },
  { "Medusa",     'M', 3, 3 },
  { "Phantasm",   'P', 2, 3 },
  { "Elemental",  'E', 2, 3 },
  { "Vampire",    'V', 3, 2 },
  { "Lich",       'L', 4, 3 },
  { "Fire Giant", 'F', 3, 3 },
  { "Basilisk",   'B', 3, 4 },
  { "Nightmare",  'N', 2, 3 }
};

// The three tiers have different lengths.
// sizeof(array) / sizeof(one element) = number of elements.
const MonsterType* const monsterTiers[3] = { tier1, tier2, tier3 };
const uint8_t tierSizes[3] = {
  sizeof(tier1) / sizeof(tier1[0]),
  sizeof(tier2) / sizeof(tier2[0]),
  sizeof(tier3) / sizeof(tier3[0])
};

// A C++ 2D array must be rectangular, so shorter rows are padded with 0 = "no monster".
const uint8_t monsterLevels[5][7] PROGMEM = {
  { 3, 3, 3, 3, 3, 3, 3 },
  { 2, 2, 2, 2, 2, 3, 3 },
  { 2, 2, 2, 2, 2, 2, 0 },
  { 1, 1, 1, 1, 2, 2, 0 },
  { 1, 1, 1, 1, 1, 0, 0 }
};


// ===== Maze layout =====
// The starting layout, kept in flash. initMaze() copies it into the
// real maze array each time.
const uint8_t mazeLayout[MAZE_H][MAZE_W] PROGMEM = {
  { 9,0,9,1,9,9,9,9,2,9 },
  { 9,1,1,4,3,1,1,4,1,9 },
  { 9,4,8,0,1,1,0,8,4,9 },
  { 9,1,0,0,0,0,0,0,1,9 },
  { 9,1,1,1,0,1,0,1,0,9 },
  { 9,0,2,0,3,1,3,0,4,1 },
  { 1,4,1,1,0,1,0,0,2,9 },
  { 9,0,8,0,0,0,0,8,1,9 },
  { 9,1,1,4,1,1,0,4,0,9 },
  { 9,9,9,9,9,9,9,1,1,9 }
};


bool inbounds(int x, int y) {
  // The maze is always MAZE_W x MAZE_H, so no len() needed
  return x >= 0 && x < MAZE_W && y >= 0 && y < MAZE_H;
}


// Randomly keeps one spot with the given value and erases the others
// (sets them to FLOOR). If eraseAll is true, the chosen one is erased too.
//
// This makes two passes: count the matching spots, pick a
// random number, then walk the maze again to find that spot.
static Point pickRandom(MapSpot maze[MAZE_H][MAZE_W], uint8_t val, bool eraseAll) {
  Point kept = { -1, -1 };

  int count = 0;
  for (int y = 0; y < MAZE_H; y++) {
    for (int x = 0; x < MAZE_W; x++) {
      if (maze[y][x].val == val) {
        count++;
      }
    }
  }
  if (count == 0) {
    return kept;
  }

  int skip = random(count);    // 0 to count-1, like randint(0, len-1)
  int index = 0;

  for (int y = 0; y < MAZE_H; y++) {
    for (int x = 0; x < MAZE_W; x++) {
      if (maze[y][x].val == val) {
        if (index == skip) {
          kept.y = y;
          kept.x = x;
        }
        if (eraseAll || index != skip) {
          maze[y][x].val = FLOOR;
        }
        index++;
      }
    }
  }
  return kept;
}


// The maze is a global array that gets passed in and filled.
void initMaze(MapSpot maze[MAZE_H][MAZE_W], Player& guy) {
  // Copy the layout from flash, and mark walls as explored
  for (int y = 0; y < MAZE_H; y++) {
    for (int x = 0; x < MAZE_W; x++) {
      maze[y][x] = MapSpot(pgm_read_byte(&mazeLayout[y][x]));
      if (maze[y][x].val == WALL) {
        maze[y][x].explored = true;
      }
    }
  }

  Point start = pickRandom(maze, START, true);
  guy.pos[0] = start.y;
  guy.pos[1] = start.x;
  maze[start.y][start.x].explored = true;

  pickRandom(maze, EXIT, false);
  pickRandom(maze, KEY, false);

  // Create a shop and make it visible
  Point shop = pickRandom(maze, SHOP, false);
  maze[shop.y][shop.x].explored = true;
}


// Returns a pointer to the monster it, or nullptr if none.
// Dead monsters are skipped, so they never block movement.
Monster* monsterAtLocation(Monster* monsters, int monsterCount, int x, int y) {
  for (int i = 0; i < monsterCount; i++) {
    if (!monsters[i].dead && monsters[i].x == x && monsters[i].y == y) {
      return &monsters[i];
    }
  }
  return nullptr;
}


bool spotEmpty(MapSpot maze[MAZE_H][MAZE_W], Monster* monsters, int monsterCount, int x, int y) {
  return inbounds(x, y) &&
         maze[y][x].val != WALL &&
         maze[y][x].val != SHOP &&
         monsterAtLocation(monsters, monsterCount, x, y) == nullptr;
}


// Could a new monster be placed here?
static bool isMonsterSpot(MapSpot maze[MAZE_H][MAZE_W], Monster* monsters, int monsterCount,
                   const Player& guy, int x, int y) {
  return maze[y][x].val == FLOOR &&
         y != guy.pos[0] && x != guy.pos[1] &&
         monsterAtLocation(monsters, monsterCount, x, y) == nullptr;
}


// The monsters go into the array passed in, and the function returns how many were added.
int monstersAdd(MapSpot maze[MAZE_H][MAZE_W], Monster* monsters, const Player& guy) {
  int count = 0;

  for (int i = 0; i < 7; i++) {
    uint8_t tier = pgm_read_byte(&monsterLevels[guy.dungeon - 1][i]);
    if (tier == 0) {
      break;                   // end of this level's list
    }

    // Count the free spots, then pick one at random (same two-pass approach as pickRandom)
    int spots = 0;
    for (int y = 0; y < MAZE_H; y++) {
      for (int x = 0; x < MAZE_W; x++) {
        if (isMonsterSpot(maze, monsters, count, guy, x, y)) {
          spots++;
        }
      }
    }
    if (spots == 0) {
      break;
    }

    int pick = random(spots);
    int spotX = 0, spotY = 0, index = 0;
    for (int y = 0; y < MAZE_H; y++) {
      for (int x = 0; x < MAZE_W; x++) {
        if (isMonsterSpot(maze, monsters, count, guy, x, y)) {
          if (index == pick) {
            spotX = x;
            spotY = y;
          }
          index++;
        }
      }
    }

    // Pick a random monster type from this tier and copy its stats out
    // of flash into a normal struct with memcpy_P.
    const MonsterType* type = &monsterTiers[tier - 1][random(tierSizes[tier - 1])];
    MonsterType stats;
    memcpy_P(&stats, type, sizeof(stats));

    // The name stays in flash: type->name is its flash address, and the
    // cast marks it as flash text so printing works correctly.
    monsters[count] = Monster((const __FlashStringHelper*)type->name,
                              stats.symbol, stats.health, stats.damage,
                              spotX, spotY);
    count++;
  }

  return count;
}


// Draw the icon for a spot in the maze
char drawSpace(uint8_t val) {
  char ch = ' ';
  if (val == WALL)       ch = '#';
  else if (val == TRAP)  ch = '!';
  else if (val == KEY)   ch = '~';
  else if (val == SHOP)  ch = '$';
  else if (val == 7)     ch = '@';   // Guy
  else if (val == EXIT)  ch = '>';
  return ch;
}


// Draws the 4x4 view around the player in the last 4 LCD columns (16-19).
// lastNSMove is 'n' or 's', lastEWMove is 'e' or 'w': single chars now,
// not strings.
void mapAddToLCD(MapSpot maze[MAZE_H][MAZE_W], Monster* monsters, int monsterCount,
                 const Player& guy, char lastNSMove, char lastEWMove) {
  int yStart = (lastNSMove == 's') ? -2 : -1;   // ?: = inline if/else
  int xStart = (lastEWMove == 'e') ? -2 : -1;

  for (int row = 0; row < 4; row++) {
    for (int col = 0; col < 4; col++) {
      int y = yStart + row;
      int x = xStart + col;
      uint8_t ch;

      if (y == 0 && x == 0) {
        ch = 0b11101111;   // Guy
      } else {
        int val;
        int xPos = guy.pos[1] + x;
        int yPos = guy.pos[0] + y;

        if (inbounds(xPos, yPos)) {
          Monster* monster = monsterAtLocation(monsters, monsterCount, xPos, yPos);
          if (monster != nullptr) {
            ch = monster->symbol;   // -> reaches a member through a pointer
            val = -2;               // Monster
          } else if (!maze[yPos][xPos].explored) {
            val = -1;
          } else {
            val = maze[yPos][xPos].val;
          }
        } else {
          val = WALL;
        }

        if (val == FLOOR)      ch = 0b10100101;
        else if (val == -1)    ch = 0b11011011;   // Unexplored
        else if (val == WALL)  ch = 0b11111111;
        else if (val == TRAP)  ch = 0b01111000;
        else if (val == KEY)   ch = 0b10101111;
        else if (val == SHOP)  ch = 0b00100100;
        else if (val == EXIT)  ch = 0b01111110;
      }

      // Replaces the two lcd_byte() calls: move the cursor, then send
      // the character code
      lcd.setCursor(16 + col, row);
      lcd.write(ch);
    }
  }
}
