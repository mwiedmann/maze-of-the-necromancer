import random
import mazeHelper as mh

class Monster:
    def __init__(self, name, symbol, health, damage, x, y):
        #   
        self.name = name
        self.symbol = symbol
        self.health = health
        self.damage = damage
        self.x = x
        self.y = y
        self.dead = False
        self.justFought = False
        self.gold = random.randrange(1, health + damage + 1)
        
    def move(self, maze, monsterList, guy):
        if not self.justFought:
            moves = []
            if mh.spotEmpty(maze, monsterList, self.x+1, self.y):
                moves.append((self.x+1, self.y))

            if mh.spotEmpty(maze, monsterList, self.x-1, self.y):
                moves.append((self.x-1, self.y))

            if mh.spotEmpty(maze, monsterList, self.x, self.y+1):
                moves.append((self.x, self.y+1))

            if mh.spotEmpty(maze, monsterList, self.x, self.y-1):
                moves.append((self.x, self.y-1))

            if len(moves) > 0:
                random.shuffle(moves)
                self.x = moves[0][0]
                self.y = moves[0][1]
        else:
            self.justFought = False
    def takeDamage(self, val):
        self.health = self.health - val

        if self.health <= 0:
            self.dead = True
            
class MapSpot:
    def __init__(self, val):
        self.val = val
        self.explored = False

class Player:
    def __init__(self, c, f, a):
        self.charClass = c
        self.fullClass = f
        self.abilities = a
        self.gold = 15
        self.maxStat = 95
        
        if self.charClass == "R":
            self.dodge = 75
            self.attack = 35
            self.run = 35
            self.startHealth = 14
            self.damage = 1
        elif self.charClass == "B":
            self.dodge = 25
            self.attack = 75
            self.run = 20
            self.startHealth = 20
            self.damage = 2
        elif self.charClass == "M":
            self.dodge = 50
            self.attack = 15
            self.run = 75
            self.startHealth = 10
            self.damage = 1
            
        self.health = self.startHealth

    key = False
    escaped = False
    pos = [0,0]
    gameOver = False
    dungeon = 5

    def skillCheck(self, skillVal):
        return random.randint(1, 100) <= skillVal
    
    def trap(self):
        dmg = 0
        if not self.skillCheck(self.dodge):
            dmg = 1
            
        self.takeDamage(dmg)
        return dmg

    def takeDamage(self, val):
        self.health = self.health - val

        if self.health <= 0:
            self.gameOver = True

    def nextDungeon(self):
        healed = False

        self.dungeon = self.dungeon - 1
        self.key = False
        self.escaped = False
        
        if self.dungeon == 0:
            self.gameOver = True
        
        if self.health < self.startHealth:
            self.health = self.health + 1
            healed = True

        return healed

    def buy(self, amount):
        if self.gold >= amount:
            self.gold = self.gold - amount
            return True
        else:
            return False

    def heal(self, val):
        self.health = self.health + val
        if self.health > self.startHealth:
            self.health = self.startHealth

    def upgradeAttack(self, val):
        self.attack = self.attack + val
        if self.attack > self.maxStat:
            self.attack = self.maxStat

    def upgradeDodge(self, val):
        self.dodge = self.dodge + val
        if self.dodge > self.maxStat:
            self.dodge = self.maxStat
        
