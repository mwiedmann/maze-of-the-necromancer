import RPi.GPIO as GPIO
import time

buttonList = []

def waitPress():
    # Wait until button is pressed
    while True:
        button = checkButtonList(False)
        if button == -1:
            time.sleep(0)
        else:
            return button

def waitRelease(button):
    # Wait until button is released
    while not GPIO.input(button):
        time.sleep(0)

def waitPressAndReleaseWithQuit(timeout):
    button = waitPress()
    startTime = time.time() # Track how long the button is held
    waitRelease(button)

    if time.time() - startTime >= timeout:
        return -1
    else:
        return button

def waitPressAndRelease():
    button = waitPress()
    waitRelease(button)

    return button

def checkButtonList(val):
    for button in buttonList:
        if GPIO.input(button) == val:
            return button

    return -1 # nothing

def init(buttons):
    global buttonList
    buttonList = buttons
    
    for button in buttonList:
        GPIO.setup(button, GPIO.IN, pull_up_down=GPIO.PUD_UP)

def cleanup():
    GPIO.cleanup(buttonList)
