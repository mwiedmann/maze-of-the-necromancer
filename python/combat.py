import lcdModule as lMod
import mzClasses as mzc
import buttonModule as bMod

def combat(monster, guy, buttonList):
    lcdText = [None,None,None,None]
    lcdText[0] = lMod.LCDLine("********************", 1)
    lcdText[1] = lMod.LCDLine("You are attacked by", 2)
    lcdText[2] = lMod.LCDLine("a " + monster.name, 2)
    lcdText[3] = lMod.LCDLine("********************", 1)

    lMod.lcd_print(lcdText)
    bMod.waitPressAndRelease()  # Wait for button
    ranAway = False
    monster.justFought = True
    
    while not monster.dead and guy.health > 0 and not ranAway:
        lcdText[0] = lMod.LCDLine(monster.name[:9] + " HP:" + str(monster.health) +
            " DMG:" + str(monster.damage), 1)
        lcdText[1] = lMod.LCDLine("You HP:" + str(guy.health) +
            " DMG:" + str(guy.damage), 1)
        lcdText[2] = lMod.LCDLine("BLUE: Atk  RED: Def", 1)
        lcdText[3] = lMod.LCDLine("YELLOW: Run", 1)

        lMod.lcd_print(lcdText)
        buttonPressed = bMod.waitPressAndRelease()  # Wait for button

        attack = guy.attack
        dodge = guy.dodge
        triedToRun = False
        
        lcdText[0] = lMod.LCDLine("You decide to try", 1)
        
        if buttonPressed == buttonList[0]: # BLUE: Attack
            attack = attack + 15
            lcdText[1] = lMod.LCDLine("an all out attack,", 1)
        elif buttonPressed == buttonList[3]: # RED: Defense
            dodge = dodge + 15
            lcdText[1] = lMod.LCDLine("a defensive strike,", 1)
        else: # Run
            lcdText[1] = lMod.LCDLine("to bravely run away,", 1)
            triedToRun = True
            ranAway = guy.skillCheck(guy.run)

        if triedToRun:
            if ranAway:
                lcdText[2] = lMod.LCDLine("and you make your", 1)
                lcdText[3] = lMod.LCDLine("escape!", 1)
            else:
                lcdText[2] = lMod.LCDLine("but the creature", 1)
                lcdText[3] = lMod.LCDLine("stops and wounds you", 1)
                guy.takeDamage(monster.damage)
        else:
            if guy.skillCheck(attack):
                monster.takeDamage(guy.damage)
                lcdText[2] = lMod.LCDLine("and you hit!", 1)
            else:
                lcdText[2] = lMod.LCDLine("and you miss.", 1)
                
            if guy.skillCheck(dodge):
                lcdText[3] = lMod.LCDLine("You swiftly dodge.", 1)
            else:
                lcdText[3] = lMod.LCDLine("You take "
                    + str(monster.damage) + " damage.", 1)
                guy.takeDamage(monster.damage)

        lMod.lcd_print(lcdText)
        bMod.waitPressAndRelease()  # Wait for button

    if monster.dead and guy.health > 0:
        lcdText[0] = lMod.LCDLine("", 2)
        lcdText[1] = lMod.LCDLine(monster.name + " defeated!", 2)
        lcdText[2] = lMod.LCDLine("You find " + str(monster.gold) + " gold", 2)
        lcdText[3] = lMod.LCDLine("", 2)

        guy.gold = guy.gold + monster.gold # Guy gets the gold
        
        lMod.lcd_print(lcdText)
        bMod.waitPressAndRelease()  # Wait for button
        
    return ranAway
