import mzClasses as mzc
import random
import lcdModule as lMod

Monsters = [
    [
        # Name, Symbol, Health, Damage
        ["Orc", "o", 1, 1],
        ["Goblin", "g", 1, 1],
        ["Slime", "s", 1, 1],
        ["Zombie", "z", 1, 1],
        ["Rat", "r", 1, 1],
        ["Skeleton", "s", 1, 1],
        ["Bat", "b", 1, 1],
        ["Swarm", "i", 1, 1],
        ["Hag", "h", 1, 1],
        ["Worg", "w", 1, 1],
        ["Kobold", "k", 1, 1]
    ],[
        ["Ogre", "O", 2, 2],
        ["Minotaur", "m", 2, 2],
        ["Imp", "I", 1, 2],
        ["Ghoul", "G", 1, 2],
        ["Werewolf", "W", 2, 1],
        ["Troll", "T", 2, 2],
        ["Ooze", "O", 2, 1],
        ["Centaur", "c", 2, 1],
        ["Yeti", "Y", 2, 2]
    ],[
        ["Dragon", "D", 4, 4],
        ["Chimera", "C", 3, 2],
        ["Medusa", "M", 3, 3],
        ["Phantasm", "P", 2, 3],
        ["Elemental", "E", 2, 3],
        ["Vampire", "V", 3, 2],
        ["Lich", "L", 4, 3],
        ["Fire Giant", "F", 3, 3],
        ["Basilisk", "B", 3, 4],
        ["Nightmare", "N", 2, 3]
    ]
]

def inbounds(maze, x, y):
    return x >= 0 and x < len(maze[0]) and y >= 0 and y < len(maze)

# The maze starts with multiple options for the starting location of
# the key, shop, the player, and the exit.
# This function randomly picks one from the list
def pickRandom(maze, list, eraseAll=False):
    skip = random.randint(0, len(list)-1)
    for erase in range(len(list)):
        if eraseAll or erase != skip:
            tup = list[erase]
            maze[tup[0]][tup[1]].val = 0

    return skip

# Create the maze and pick the random starting points
def initMaze(guy):
    maze = [[9,0,9,1,9,9,9,9,2,9],
            [9,1,1,4,3,1,1,4,1,9],
            [9,4,8,0,1,1,0,8,4,9],
            [9,1,0,0,0,0,0,0,1,9],
            [9,1,1,1,0,1,0,1,0,9],
            [9,0,2,0,3,1,3,0,4,1],
            [1,4,1,1,0,1,0,0,2,9],
            [9,0,8,0,0,0,0,8,1,9],
            [9,1,1,4,1,1,0,4,0,9],
            [9,9,9,9,9,9,9,1,1,9]]

    startingPoints = []
    exitPoints = []
    keys = []
    shops = []

    # Add some extra properties like "explored"
    for y in range(len(maze)):
        for x in range(len(maze[y])):
            maze[y][x] = mzc.MapSpot(maze[y][x])
            
    # Loop through the maze and build up lists of
    # the various starting points for key/player/exit
    for y in range(len(maze)):
        for x in range(len(maze[y])):
            if maze[y][x].val == 1: # Walls
                maze[y][x].explored = True
            if maze[y][x].val == 3: # Key spots
                keys.append((y, x))
            if maze[y][x].val == 4: # Shop spots
                shops.append((y, x))
            elif maze[y][x].val == 8: # Starting points
                startingPoints.append((y, x))
            elif maze[y][x].val == 9: # Exits
                exitPoints.append((y,x))

            

    start = pickRandom(maze, startingPoints, True)
    guy.pos[0] = startingPoints[start][0]
    guy.pos[1] = startingPoints[start][1]

    maze[guy.pos[0]][guy.pos[1]].explored = True
    
    pickRandom(maze, exitPoints)
    pickRandom(maze, keys)

    # Create a shop and make it visible
    shopSpot = pickRandom(maze, shops)
    maze[shops[shopSpot][0]][shops[shopSpot][1]].explored = True
    
    return maze

def monstersAdd(maze, guy):
    monsterSpots = []
    monsterList = []

    monsterLevels = [
        [3,3,3,3,3,3,3],
        [2,2,2,2,2,3,3],
        [2,2,2,2,2,2],
        [1,1,1,1,2,2],
        [1,1,1,1,1]
    ]
    
    for y in range(len(maze)):
        for x in range(len(maze[y])):
            if maze[y][x].val == 0 and y != guy.pos[0] and x != guy.pos[1]:
                monsterSpots.append((y, x))

    levels = monsterLevels[guy.dungeon - 1]
    
    for c in range(len(levels)):
        rndSpot = random.randrange(0, len(monsterSpots))
        monsterChoices = Monsters[levels[c]-1]
        monsterStats = monsterChoices[random.randrange(0, len(monsterChoices))]

        # Name, Symbol, Health, Damage, x, y
        monsterList.append(mzc.Monster(monsterStats[0], monsterStats[1],
            monsterStats[2], monsterStats[3],
            monsterSpots[rndSpot][1], monsterSpots[rndSpot][0]))
        monsterSpots.remove(monsterSpots[rndSpot])

    return monsterList

def monsterAtLocation(monsterList, x, y):
    for m in monsterList:
        if m.x == x and m.y == y:
            return m

    return None

def spotEmpty(maze, monsterList, x, y):
    return (inbounds(maze, x, y) and
        maze[y][x].val != 1 and # Walls
        maze[y][x].val != 4 and # Shop
        not monsterAtLocation(monsterList, x, y))
        

# Draw the maze to the console
def drawMaze(maze, monsterList, guy):
    for y in range(len(maze)):
        if y == 0:
            drawBorderTopBottom(len(maze[y]))

        print ("|", end="")
        for x in range(len(maze[y])):
            if y == guy.pos[0] and x == guy.pos[1]:
                print (drawSpace(7), end="")
            else:
                monster = monsterAtLocation(monsterList, x, y)
                if not monster == None:
                    print (monster.symbol, end="")
                else:
                    print (drawSpace(maze[y][x].val), end="")
                

        print ("|")

        if y == len(maze) - 1:
            drawBorderTopBottom(len(maze[y]))

# Helper to draw a border
def drawBorderTopBottom(length):
    for d in range(length + 2):
        print ("-", end="")
    print()

# Draw the icon for a spot in the maze
def drawSpace(val):
    char = " "
    if val == 1:    # Walls
        char = "#"
    elif val == 2:  # Traps
        char = "!"
    elif val == 3:  # Keys
        char = "~"
    elif val == 4:  # Keys
        char = "$"
    elif val == 7:  # Guy
        char = "@"
    #elif val == 8: # Starting spot (prob don't need to draw this)
    #    char = "S"
    elif val == 9:  # Exit
        char = ">"

    return char

def drawRangeGet(southOrEast):
    if southOrEast:
        return [-2,-1,0,1]
    else:
        return [-1,0,1,2]
    
def mapAddToLCD (maze, monsterList, guy, lastNSMove, lastEWMove):
    
    yRange = drawRangeGet(lastNSMove == "s")
    lcdYAdj = -yRange[0]
    xRange = drawRangeGet(lastEWMove == "e")
    
    for y in yRange:
        lcdXAdj = 0
        for x in xRange:
            if y == 0 and x == 0:
                char = 0b11101111 # Guy
            else:
                val = 0
                xPos = guy.pos[1] + x
                yPos = guy.pos[0] + y
                
                if xPos >= 0 and xPos < len(maze[0]) and yPos >= 0 and yPos < len(maze):
                    monster = monsterAtLocation(monsterList, xPos, yPos)
                    if not monster == None:
                        char = ord(monster.symbol)
                        val = -2 # Monster
                    elif not maze[yPos][xPos].explored:
                        val = -1
                    else:
                        val = maze[yPos][xPos].val
                else:
                    val = 1

                if val == 0:    # Open floor
                    char = 0b10100101   #0b11011011 - this one is more of a square
                elif val == -1: # Unexplored
                    char = 0b11011011   #0b00111111 Question mark
                elif val == 1:  # Walls
                    char = 0b11111111
                elif val == 2:  # Traps
                    char = 0b01111000
                elif val == 3:  # Key
                    char = 0b10101111
                elif val == 4:  # Shop
                    char = 0b00100100
                elif val == 9:  # Exit
                    char = 0b01111110

            lMod.lcd_byte(lMod.LCD_LINES[y+lcdYAdj] + 16 + lcdXAdj, lMod.LCD_CMD)
            lMod.lcd_byte(char, lMod.LCD_CHR)

            lcdXAdj = lcdXAdj + 1

