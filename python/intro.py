import lcdModule as lMod
import buttonModule as bMod
import mzClasses as mzc

def setupPlayer(buttonList):
    lcdText = [None,None,None,None]
    
    lcdText[0] = lMod.LCDLine("Welcome to", 2)
    lcdText[1] = lMod.LCDLine("The Maze of the", 2)
    lcdText[2] = lMod.LCDLine("Necromancer", 2)
    lcdText[3] = lMod.LCDLine("", 1)

    lMod.lcd_print(lcdText)
    bMod.waitPressAndRelease()  # Wait for button

    lcdText[0] = lMod.LCDLine("MENU", 2)
    lcdText[1] = lMod.LCDLine("--------------------", 2)
    lcdText[2] = lMod.LCDLine("BLUE: See intro", 2)
    lcdText[3] = lMod.LCDLine("RED: Skip to game", 2)
    
    lMod.lcd_print(lcdText)
    buttonPressed = bMod.waitPressAndRelease()  # Wait for button

    if buttonPressed == buttonList[0]:
        lcdText[0] = lMod.LCDLine("You are trapped in ", 1)
        lcdText[1] = lMod.LCDLine("the depths of the", 1)
        lcdText[2] = lMod.LCDLine("evil Necromancer's", 1)
        lcdText[3] = lMod.LCDLine("dungeon!", 1)

        lMod.lcd_print(lcdText)
        bMod.waitPressAndRelease()  # Wait for button
        
        lcdText[0] = lMod.LCDLine("Can you make your", 1)
        lcdText[1] = lMod.LCDLine("way back to the", 1)
        lcdText[2] = lMod.LCDLine("surface and gain", 1)
        lcdText[3] = lMod.LCDLine("your freedom?", 1)

        lMod.lcd_print(lcdText)
        bMod.waitPressAndRelease()  # Wait for button
        
        lcdText[0] = lMod.LCDLine("Find the key and the", 1)
        lcdText[1] = lMod.LCDLine("exit on each level.", 1)
        lcdText[2] = lMod.LCDLine("Beware of monsters", 1)
        lcdText[3] = lMod.LCDLine("and traps!", 1)

        lMod.lcd_print(lcdText)
        bMod.waitPressAndRelease()  # Wait for button

    # End of intro    
    lcdText[0] = lMod.LCDLine("Choose your class:  ", 1)
    lcdText[1] = lMod.LCDLine("RED: Rogue", 1)
    lcdText[2] = lMod.LCDLine("BLUE: Barbarian", 1)
    lcdText[3] = lMod.LCDLine("YELLOW: Mystic", 1)

    lMod.lcd_print(lcdText)
    buttonPressed = bMod.waitPressAndRelease()  # Wait for button
    
    if buttonPressed == buttonList[0]:
        charClass = "B"
        fullClass = "Barbarian"
        abilities = "Tough but slow"
    elif buttonPressed == buttonList[1]:
        charClass = "M"
        fullClass = "Mystic"
        abilities = "Invisibility"
    else:
        charClass = "R"
        fullClass = "Rogue"
        abilities = "Evasive"

    lcdText[0] = lMod.LCDLine("You have selected", 1)
    lcdText[1] = lMod.LCDLine(fullClass, 1)
    lcdText[2] = lMod.LCDLine("Ability: ", 1)
    lcdText[3] = lMod.LCDLine(abilities, 1)

    lMod.lcd_print(lcdText)
    bMod.waitPressAndRelease()  # Wait for button
    lMod.lcd_clear()
    
    return mzc.Player(charClass, fullClass, abilities)
    
