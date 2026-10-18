import RPi.GPIO as GPIO
import time
 
# Define GPIO to LCD mapping
LCD_RS = 7 # CE1
LCD_E  = 8 # CE0
LCD_D4 = 25
LCD_D5 = 24
LCD_D6 = 23
LCD_D7 = 18
LED_ON = 15
 
# Define some device constants
LCD_WIDTH = 20    # Maximum characters per line
CUR_LCD_WIDTH = LCD_WIDTH
LCD_CHR = True
LCD_CMD = False
 
LCD_LINE_1 = 0x80 # LCD RAM address for the 1st line
LCD_LINE_2 = 0xC0 # LCD RAM address for the 2nd line
LCD_LINE_3 = 0x94 # LCD RAM address for the 3rd line
LCD_LINE_4 = 0xD4 # LCD RAM address for the 4th line

LCD_LINES = [LCD_LINE_1, LCD_LINE_2, LCD_LINE_3, LCD_LINE_4]

# Timing constants
E_PULSE = 0.0005
E_DELAY = 0.0005

class LCDLine:
  def __init__(self, text, style):
    self.text = text
    self.style = style

  text = ""
  style = 1

def lcd_setup():
  # Main program block
 
  #GPIO.setmode(GPIO.BCM)       # Use BCM GPIO numbers
  GPIO.setup(LCD_E, GPIO.OUT)  # E
  GPIO.setup(LCD_RS, GPIO.OUT) # RS
  GPIO.setup(LCD_D4, GPIO.OUT) # DB4
  GPIO.setup(LCD_D5, GPIO.OUT) # DB5
  GPIO.setup(LCD_D6, GPIO.OUT) # DB6
  GPIO.setup(LCD_D7, GPIO.OUT) # DB7
  GPIO.setup(LED_ON, GPIO.OUT) # Backlight enable
 
  # Initialise display
  lcd_init()
 
def lcd_init():
  # Initialise display
  lcd_byte(0x33,LCD_CMD) # 110011 Initialise
  lcd_byte(0x32,LCD_CMD) # 110010 Initialise
  lcd_byte(0x06,LCD_CMD) # 000110 Cursor move direction
  lcd_byte(0x0C,LCD_CMD) # 001100 Display On,Cursor Off, Blink Off
  lcd_byte(0x28,LCD_CMD) # 101000 Data length, number of lines, font size
  lcd_byte(0x01,LCD_CMD) # 000001 Clear display
  time.sleep(E_DELAY)
 
def lcd_byte(bits, mode):
  # Send byte to data pins
  # bits = data
  # mode = True  for character
  #        False for command
 
  GPIO.output(LCD_RS, mode) # RS
 
  # High bits
  GPIO.output(LCD_D4, False)
  GPIO.output(LCD_D5, False)
  GPIO.output(LCD_D6, False)
  GPIO.output(LCD_D7, False)
  if bits&0x10==0x10:
    GPIO.output(LCD_D4, True)
  if bits&0x20==0x20:
    GPIO.output(LCD_D5, True)
  if bits&0x40==0x40:
    GPIO.output(LCD_D6, True)
  if bits&0x80==0x80:
    GPIO.output(LCD_D7, True)
 
  # Toggle 'Enable' pin
  lcd_toggle_enable()
 
  # Low bits
  GPIO.output(LCD_D4, False)
  GPIO.output(LCD_D5, False)
  GPIO.output(LCD_D6, False)
  GPIO.output(LCD_D7, False)
  if bits&0x01==0x01:
    GPIO.output(LCD_D4, True)
  if bits&0x02==0x02:
    GPIO.output(LCD_D5, True)
  if bits&0x04==0x04:
    GPIO.output(LCD_D6, True)
  if bits&0x08==0x08:
    GPIO.output(LCD_D7, True)
 
  # Toggle 'Enable' pin
  lcd_toggle_enable()
 
def lcd_toggle_enable():
  # Toggle enable
  time.sleep(E_DELAY)
  GPIO.output(LCD_E, True)
  time.sleep(E_PULSE)
  GPIO.output(LCD_E, False)
  time.sleep(E_DELAY)
 
def lcd_string(message,line,style):
  # Send string to display
  # style=1 Left justified
  # style=2 Centred
  # style=3 Right justified
 
  if style==1:
    message = message.ljust(CUR_LCD_WIDTH," ")
  elif style==2:
    message = message.center(CUR_LCD_WIDTH," ")
  elif style==3:
    message = message.rjust(CUR_LCD_WIDTH," ")
 
  lcd_byte(line, LCD_CMD)
 
  for i in range(CUR_LCD_WIDTH):
    lcd_byte(ord(message[i]),LCD_CHR)

def lcd_print(messageList):
  for n in range(len(messageList)):
    print (messageList[n].text)
    lcd_string(messageList[n].text, LCD_LINES[n], messageList[n].style) 
  
def lcd_backlight(flag):
  # Toggle backlight on-off-on
  GPIO.output(LED_ON, flag)

def lcd_cleanup():
  lcd_byte(0x01, LCD_CMD)
  GPIO.cleanup()

def lcd_clear():
  lcd_string("", LCD_LINE_1, 1)
  lcd_string("", LCD_LINE_2, 1)
  lcd_string("", LCD_LINE_3, 1)
  lcd_string("", LCD_LINE_4, 1)
  
