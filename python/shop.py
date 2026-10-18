import lcdModule as lMod
import mzClasses as mzc
import buttonModule as bMod

lcdText = [None,None,None,None]

def notEnoughGold():
    lcdText[0] = lMod.LCDLine("", 2)
    lcdText[1] = lMod.LCDLine("Hmm, you don't have", 2)
    lcdText[2] = lMod.LCDLine("enough gold for that", 2)
    lcdText[3] = lMod.LCDLine("", 1)

    lMod.lcd_print(lcdText)
    bMod.waitPressAndRelease()  # Wait for button

def guyBuy(guy, cost, buttonList, lcdText):
    noBuyVal = 3
    lMod.lcd_print(lcdText)
    buttonPressed = bMod.waitPressAndRelease()  # Wait for button

    if buttonPressed == buttonList[3]:
        return noBuyVal
    else:
        for i in range(len(cost)):
            if buttonPressed == buttonList[i]:
                if guy.buy(cost[i]):
                    return i
                else:
                    notEnoughGold()
                    return noBuyVal
    
def shop(guy, buttonList):
    quitSelection = 3
    
    lcdText[0] = lMod.LCDLine("You approach an old", 1)
    lcdText[1] = lMod.LCDLine("woman. She smiles at", 1)
    lcdText[2] = lMod.LCDLine("you and says, 'I'll", 1)
    lcdText[3] = lMod.LCDLine("help...for a price'", 1)

    lMod.lcd_print(lcdText)
    bMod.waitPressAndRelease()  # Wait for button

    while True:
        lcdText[0] = lMod.LCDLine("BLUE: Healing", 1)
        lcdText[1] = lMod.LCDLine("YELLOW: Attack", 1)
        lcdText[2] = lMod.LCDLine("GREEN: Dodge", 1)
        lcdText[3] = lMod.LCDLine("RED: BACK  Gold:" + str(guy.gold), 1)

        lMod.lcd_print(lcdText)
        buttonPressed = bMod.waitPressAndRelease()  # Wait for button

        if buttonPressed == buttonList[0]:
            selection = 0

            while selection != quitSelection:
                cost = [2,3,5]
                vals = [2,4,8]
                lcdText[0] = lMod.LCDLine("BLUE: Heal 2   [" + str(cost[0]) + "g]", 1)
                lcdText[1] = lMod.LCDLine(" YLW: Heal 4   [" + str(cost[1]) + "g]", 1)
                lcdText[2] = lMod.LCDLine("GREN: Heal 8   [" + str(cost[2]) + "g]", 1)
                lcdText[3] = lMod.LCDLine("Gold:" + str(guy.gold) + " HP:" + str(guy.health) +
                                          "/" + str(guy.startHealth), 1)
            
                selection = guyBuy(guy, cost, buttonList, lcdText)

                if selection < 3:
                    guy.heal(vals[selection])

        elif buttonPressed == buttonList[1]:
            selection = 0

            while selection != quitSelection:
                cost = [2,5,8]
                vals = [1,3,5]
                lcdText[0] = lMod.LCDLine("BLUE: Attack +1 [" + str(cost[0]) + "g]", 1)
                lcdText[1] = lMod.LCDLine(" YLW: Attack +3 [" + str(cost[1]) + "g]", 1)
                lcdText[2] = lMod.LCDLine("GREN: Attack +5 [" + str(cost[2]) + "g]", 1)
                lcdText[3] = lMod.LCDLine("Gold:" + str(guy.gold) + " Attack:" + str(guy.attack), 1)
            
                selection = guyBuy(guy, cost, buttonList, lcdText)

                if selection < 3:
                    guy.upgradeAttack(vals[selection])

        elif buttonPressed == buttonList[2]:
            selection = 0

            while selection != quitSelection:
                cost = [2,5,8]
                vals = [1,3,5]
                lcdText[0] = lMod.LCDLine("BLUE: Dodge +1 [" + str(cost[0]) + "g]", 1)
                lcdText[1] = lMod.LCDLine(" YLW: Dodge +3 [" + str(cost[1]) + "g]", 1)
                lcdText[2] = lMod.LCDLine("GREN: Dodge +5 [" + str(cost[2]) + "g]", 1)
                lcdText[3] = lMod.LCDLine("Gold:" + str(guy.gold) + " Dodge:" + str(guy.dodge), 1)
            
                selection = guyBuy(guy, cost, buttonList, lcdText)

                if selection < 3:
                    guy.upgradeDodge(vals[selection])
                            
        else:
            return
