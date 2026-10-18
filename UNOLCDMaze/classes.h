#pragma once
#include <Arduino.h>

// ===== Sizes =====
// Placeholders: change these to match your actual maze.
const int MAZE_W = 10;
const int MAZE_H = 10;
const int MAX_MONSTERS = 7;

// plenty for a 10x10 maze.
struct Point {
  int8_t y;
  int8_t x;
};

// ===== MapSpot =====
// Defined first because Monster::move() needs to know what a MapSpot is.
class MapSpot {
  public:
    uint8_t val;
    bool explored;

    MapSpot() : val(0), explored(false) {}
    MapSpot(uint8_t v) : val(v), explored(false) {}
};


// ===== Monster =====
class Monster {
  public:
    const __FlashStringHelper* name;   // was: const char* name  // points to a string literal like "Goblin"
    char symbol;       // single char: use 'g', not "g"
    int health;
    int damage;
    int x, y;
    bool dead;
    bool justFought;
    int gold;

    // Empty slot for the monster array. dead = true means unused slots
    // are skipped by any code that checks for dead monsters.
    Monster() : name(nullptr), symbol(' '), health(0), damage(0),
                x(0), y(0), dead(true), justFought(false), gold(0) {}

    Monster(const __FlashStringHelper* name, char symbol, int health, int damage, int x, int y)
      : name(name), symbol(symbol), health(health), damage(damage),
        x(x), y(y), dead(false), justFought(false) {
      // random(min, max) excludes max, exactly like Python's randrange
      gold = random(1, health + damage + 1);
    }

    void move(MapSpot maze[MAZE_H][MAZE_W], Monster* monsters, int monsterCount);

    void takeDamage(int val) {
      health -= val;
      if (health <= 0) {
        dead = true;
      }
    }
};

// ===== Player =====
class Player {
  public:
    char charClass;          // 'R', 'B' or 'M'
    const char* fullClass;   // e.g. "Rogue"
    const char* abilities;

    int gold = 15;
    int maxStat = 95;
    int dodge = 0;
    int attack = 0;
    int run = 0;
    int startHealth = 0;
    int damage = 0;
    int health = 0;

    bool key = false;
    bool escaped = false;
    int pos[2] = { 0, 0 };
    bool gameOver = false;
    int dungeon = 5;

    Player() : charClass(' '), fullClass(""), abilities("") {}

    Player(char c, const char* f, const char* a)
      : charClass(c), fullClass(f), abilities(a) {
      if (charClass == 'R') {
        dodge = 75;  attack = 35;  run = 35;  startHealth = 14;  damage = 1;
      } else if (charClass == 'B') {
        dodge = 25;  attack = 75;  run = 20;  startHealth = 20;  damage = 2;
      } else if (charClass == 'M') {
        dodge = 50;  attack = 15;  run = 75;  startHealth = 10;  damage = 1;
      }
      health = startHealth;
    }

    bool skillCheck(int skillVal) {
      return random(1, 101) <= skillVal;
    }

    int trap() {
      int dmg = 0;
      if (!skillCheck(dodge)) {
        dmg = 1;
      }
      takeDamage(dmg);
      return dmg;
    }

    void takeDamage(int val) {
      health -= val;
      if (health <= 0) {
        gameOver = true;
      }
    }

    bool nextDungeon() {
      bool healed = false;

      dungeon--;
      key = false;
      escaped = false;

      if (dungeon == 0) {
        gameOver = true;
      }

      if (health < startHealth) {
        health++;
        healed = true;
      }

      return healed;
    }

    bool buy(int amount) {
      if (gold >= amount) {
        gold -= amount;
        return true;
      }
      return false;
    }

    void heal(int val) {
      health += val;
      if (health > startHealth) {
        health = startHealth;
      }
    }

    void upgradeAttack(int val) {
      attack += val;
      if (attack > maxStat) {
        attack = maxStat;
      }
    }

    void upgradeDodge(int val) {
      dodge += val;
      if (dodge > maxStat) {
        dodge = maxStat;
      }
    }
};