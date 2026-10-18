#pragma once
#include "classes.h"

const uint8_t FLOOR = 0;
const uint8_t WALL  = 1;
const uint8_t TRAP  = 2;
const uint8_t KEY   = 3;
const uint8_t SHOP  = 4;
const uint8_t START = 8;
const uint8_t EXIT  = 9;

bool inbounds(int x, int y);
void initMaze(MapSpot maze[MAZE_H][MAZE_W], Player& guy);
Monster* monsterAtLocation(Monster* monsters, int monsterCount, int x, int y);
bool spotEmpty(MapSpot maze[MAZE_H][MAZE_W], Monster* monsters, int monsterCount, int x, int y);
int monstersAdd(MapSpot maze[MAZE_H][MAZE_W], Monster* monsters, const Player& guy);
char drawSpace(uint8_t val);
void mapAddToLCD(MapSpot maze[MAZE_H][MAZE_W], Monster* monsters, int monsterCount,
                 const Player& guy, char lastNSMove, char lastEWMove);
