#include "classes.h"
#include "maze.h"        // for spotEmpty()

void Monster::move(MapSpot maze[MAZE_H][MAZE_W], Monster* monsters, int monsterCount) {
  if (justFought) {
    justFought = false;
    return;
  }

  int movesX[4];
  int movesY[4];
  int count = 0;

  // The four directions: right, left, down, up
  const int dx[4] = { 1, -1, 0,  0 };
  const int dy[4] = { 0,  0, 1, -1 };

  for (int i = 0; i < 4; i++) {
    int nx = x + dx[i];
    int ny = y + dy[i];
    if (spotEmpty(maze, monsters, monsterCount, nx, ny)) {
      movesX[count] = nx;
      movesY[count] = ny;
      count++;
    }
  }

  // Shuffling and taking the first element is the same as picking one
  // at random, so this skips the shuffle.
  if (count > 0) {
    int pick = random(count);   // 0 to count-1
    x = movesX[pick];
    y = movesY[pick];
  }
}

