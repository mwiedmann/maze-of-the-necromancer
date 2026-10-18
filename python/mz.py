import RPi.GPIO as GPIO
import random
import time
import lcdModule as lMod
import buttonModule as bMod
import mzClasses as mzc
import mazeHelper as mh
import combat
import shop
import intro

lcdText = [None,None,None,None]
buttonLeft = 12 # Blue
buttonUp = 16 # Yellow
buttonDown = 20 # Red
buttonRight = 21 # Green

buttonList = [buttonLeft, buttonUp, buttonDown, buttonRight]

showConsoleMap = True

def lcdTextClear():
    global lcdText
    for i in range(len(lcdText)):
        lcdText[i] = lMod.LCDLine("", 1)
    
# See if the requested player move is valid. Move if it is.
def moveGuy(maze, guy):
    buttonPressed = bMod.waitPressAndReleaseWithQuit(2)

    if buttonPressed == buttonLeft:
        move = "w"
        moveDesc = "West"
    elif buttonPressed == buttonUp:
        move = "n"
        moveDesc = "North"
    elif buttonPressed == buttonDown:
        move = "s"
        moveDesc = "South"
    elif buttonPressed == buttonRight:
        move = "e"
        moveDesc = "East"
    elif buttonPressed == -1:
        move = "q"

    #lMod.lcd_clear()
    #lMod.lcd_print([lMod.LCDLine(moveDesc, 1)])
    #time.sleep(1)
    
    y = guy.pos[0]
    x = guy.pos[1]

    if move == "n":
        y = y - 1
    elif move == "s":
        y = y + 1
    elif move == "w":
        x = x - 1
    elif move == "e":
        x = x + 1
    elif move == "q":
        guy.gameOver = True
        
    if y >= 0 and y < len(maze) and x >= 0 and x < len(maze[0]) and maze[y][x].val != 1:
        guy.pos[0] = y
        guy.pos[1] = x
        maze[y][x].explored = True

    return move

def moveMonsters(maze, monsterList, guy):
    for monster in monsterList:
        if monster.justFought: # Monster won't move if just fought
            monster.justFought = False
        else:
            monster.move(maze, monsterList, guy)
        
def movePlayerToEmpty(maze, monsterList, guy):
    options = []

    if mh.spotEmpty(maze, monsterList, guy.pos[1]+1, guy.pos[0]):
        options.append((guy.pos[0], guy.pos[1]+1))

    if mh.spotEmpty(maze, monsterList, guy.pos[1]-1, guy.pos[0]):
        options.append((guy.pos[0], guy.pos[1]-1))

    if mh.spotEmpty(maze, monsterList, guy.pos[1], guy.pos[0]+1):
        options.append((guy.pos[0]+1, guy.pos[1]))

    if mh.spotEmpty(maze, monsterList, guy.pos[1], guy.pos[0]-1):
        options.append((guy.pos[0]-1, guy.pos[1]))

    newSpot = options[random.randrange(0, len(options))]
    guy.pos[0] = newSpot[0]
    guy.pos[1] = newSpot[1]

def checkForMonsters(maze, monsterList, guy):
    monster = mh.monsterAtLocation(monsterList, guy.pos[1], guy.pos[0])

    if not monster == None:
        if not monster.justFought:
            result = combat.combat(monster, guy, buttonList)
            if result:
                movePlayerToEmpty(maze, monsterList, guy)

            if monster.dead:
                monsterList.remove(monster)
    
# Check the player's current spot.
# Set some status variables if standing on key/trap/exit
def checkMoveResult(maze, monsterList, guy):
    val = maze[guy.pos[0]][guy.pos[1]].val
    lcdText[3] = lMod.LCDLine("", 1)

    if val == 2:	# Traps
        if guy.trap() > 0:
            lcdText[3] = lMod.LCDLine("A TRAP! OUCH!", 1)
        else:
            lcdText[3] = lMod.LCDLine("Trap dodged!", 1)
    elif val == 3:	# Keys
        lcdText[3] = lMod.LCDLine("Found a KEY!", 1)
        maze[guy.pos[0]][guy.pos[1]].val = 0 # Remove key from map
        guy.key = True
    elif val == 4:
        shop.shop(guy, buttonList)
    elif val == 9:	# Exit
        if guy.key:
            guy.escaped = True
        else:
            lcdText[3] = lMod.LCDLine("Exit, no key!", 1)

def moveDirsGet(maze, guy):
    text = ""
    if mh.inbounds(maze, guy.pos[1], guy.pos[0]-1) and maze[guy.pos[0]-1][guy.pos[1]].val != 1:
        text = text + "N "
    if mh.inbounds(maze, guy.pos[1], guy.pos[0]+1) and maze[guy.pos[0]+1][guy.pos[1]].val != 1:
        text = text + "S "
    if mh.inbounds(maze, guy.pos[1]+1, guy.pos[0]) and maze[guy.pos[0]][guy.pos[1]+1].val != 1:
        text = text + "E "
    if mh.inbounds(maze, guy.pos[1]-1, guy.pos[0]) and maze[guy.pos[0]][guy.pos[1]-1].val != 1:
        text = text + "W "

    return text.strip()

##############################################
# Main Prog Begins
##############################################

GPIO.setmode(GPIO.BCM)
GPIO.setwarnings(False)

lMod.lcd_setup()
bMod.init(buttonList)
lcdTextClear()

guy = intro.setupPlayer(buttonList)

while not guy.gameOver :
    maze = mh.initMaze(guy)
    monsterList = mh.monstersAdd(maze, guy)

    # Need these for how the map is drawn to LCD
    lastNSMove = "n" 
    lastEWMove = "e"
    
    while not guy.gameOver and not guy.escaped :
        lMod.CUR_LCD_WIDTH = 16
        lcdText[0] = lMod.LCDLine("{" + str(guy.dungeon) + "}" +
                                  " HP:" + str(guy.health) +
                                  " DMG:" + str(guy.damage), 1)
        lcdText[1] = lMod.LCDLine("ATK:" + str(guy.attack) +
                                  " DODGE:" + str(guy.dodge), 1)
        invLine = "GOLD:" + str(guy.gold)
            
        if guy.key:
            invLine = invLine + " KEY"

        lcdText[2] = lMod.LCDLine(invLine, 1)

        lMod.lcd_print(lcdText)
        lMod.CUR_LCD_WIDTH = 20
        
        # Show current map
        mh.mapAddToLCD(maze, monsterList, guy, lastNSMove, lastEWMove)
        if showConsoleMap:
            mh.drawMaze(maze, monsterList, guy)
        
        move = moveGuy(maze, guy)

        # We track the moves to help with LCD map positioning
        if move == "e":
            lastEWMove = "e"
        elif move == "w":
            lastEWMove = "w"
        elif move == "n":
            lastNSMove = "n"
        else:
            lastNSMove = "s"

        if move != "q":
            checkForMonsters(maze, monsterList, guy) # Check after guy moves
            moveMonsters(maze, monsterList, guy)
            checkForMonsters(maze, monsterList, guy) # Check after monsters move
        
            checkMoveResult(maze, monsterList, guy) # Check traps, etc.
        
    if guy.escaped and guy.dungeon > 0:
        lcdText[0] = lMod.LCDLine("You escaped to the ", 1)
        lcdText[1] = lMod.LCDLine("next level!!!", 1)
        if guy.nextDungeon():
            lcdText[2] = lMod.LCDLine("You rest a minute", 1)
            lcdText[3] = lMod.LCDLine("and heal slightly.", 1)
        else:
            lcdText[2] = lMod.LCDLine("", 1)
            lcdText[3] = lMod.LCDLine("", 1)
            
        lMod.lcd_print(lcdText)
        bMod.waitPressAndRelease()  # Wait for button
        lcdTextClear()
            
if guy.dungeon == 0:
    lcdText[0] = lMod.LCDLine("You have made it out", 1)
    lcdText[1] = lMod.LCDLine("to the surface! YES!", 1)
    lcdText[2] = lMod.LCDLine("The Necromancer", 1)
    lcdText[3] = lMod.LCDLine("appears nearby.", 1)

    lMod.lcd_print(lcdText)
    bMod.waitPressAndRelease()  # Wait for button

    lcdText[0] = lMod.LCDLine("'I see you have", 1)
    lcdText[1] = lMod.LCDLine("passed my test', he", 1)
    lcdText[2] = lMod.LCDLine("says as he smiles.", 1)
    lcdText[3] = lMod.LCDLine("You are confused.", 1)

    lMod.lcd_print(lcdText)
    bMod.waitPressAndRelease()  # Wait for button

    lcdText[0] = lMod.LCDLine("As he starts to cast", 1)
    lcdText[1] = lMod.LCDLine("a spell, he says    ", 1)
    lcdText[2] = lMod.LCDLine("'It is time for your", 1)
    lcdText[3] = lMod.LCDLine("next adventure!'    ", 1)

    lMod.lcd_print(lcdText)
    bMod.waitPressAndRelease()  # Wait for button

    lcdText[0] = lMod.LCDLine("To be continued in", 2)
    lcdText[1] = lMod.LCDLine("Maze of the", 2)
    lcdText[2] = lMod.LCDLine("Necromancer II", 2)
    lcdText[3] = lMod.LCDLine("", 2)

    lMod.lcd_print(lcdText)
    bMod.waitPressAndRelease()  # Wait for button

    lcdText[0] = lMod.LCDLine("Programming by", 2)
    lcdText[1] = lMod.LCDLine("Mark Wiedmann", 2)
    lcdText[2] = lMod.LCDLine("", 2)
    lcdText[3] = lMod.LCDLine("Thanks for playing!", 2)

    lMod.lcd_print(lcdText)
    bMod.waitPressAndRelease()  # Wait for button
else:
    if move != "q":
        lcdText[0] = lMod.LCDLine("As you take your", 1)
        lcdText[1] = lMod.LCDLine("final breath,", 1)
        lcdText[2] = lMod.LCDLine("the Necromancer's", 1)
        lcdText[3] = lMod.LCDLine("voice echoes...", 1)

        lMod.lcd_print(lcdText)
        bMod.waitPressAndRelease()  # Wait for button

        lcdText[0] = lMod.LCDLine("'Hmm, such a shame.", 1)
        lcdText[1] = lMod.LCDLine("I had very high", 1)
        lcdText[2] = lMod.LCDLine("expectations for", 1)
        lcdText[3] = lMod.LCDLine("you.'", 1)

        lMod.lcd_print(lcdText)
        bMod.waitPressAndRelease()  # Wait for button

lMod.lcd_cleanup()

