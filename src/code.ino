#include <avr/sleep.h>
#include <EEPROM.h>

#define LED_1 3
#define LED_2 5
#define LED_4 6
#define LED_8 9
#define BTN_1 2
#define BTN_2 4
#define BTN_4 7
#define BTN_8 8

#define FLASH_TIME 300
#define TIMEOUT_MS 30000

char counter = 0;
char streak = -1;
char sequence[16];
bool waitForUnpress = false;
unsigned long lastInteraction = 0;

char highscore = 0;

// makes accessing the LEDs a lot easier when iterating
char ledMap[] = {LED_1, LED_2, LED_4, LED_8};

// setup is called once when the device starts
void setup()
{
  pinMode(LED_1, OUTPUT);
  pinMode(LED_2, OUTPUT);
  pinMode(LED_4, OUTPUT);
  pinMode(LED_8, OUTPUT);
  pinMode(BTN_1, INPUT_PULLUP);
  pinMode(BTN_2, INPUT_PULLUP);
  pinMode(BTN_4, INPUT_PULLUP);
  pinMode(BTN_8, INPUT_PULLUP);

  // check whether all buttons are pressed for 4s and wipe EEPROM if this happens
  checkReset();

  // initalize the random function
  randomSeed(analogRead(A0));

  swipeLight(50, true);

  // if the max score (15) was reached we want a special effect
  highscore = EEPROM.read(0);
  if (highscore > 0) {
    delay(300);
    if (highscore == 15) {
      displayScore(highscore, 2., 4);
      delay(200);
    } else {
      displayScore(highscore, 0.5, 1);
    }
    swipeLight(50, false);
  }
  delay(500);
}

// checks whether all buttons are pressed simultaniously
int checkResetButtons() {
  bool val1 = digitalRead(BTN_1) == LOW;
  bool val2 = digitalRead(BTN_2) == LOW;
  bool val4 = digitalRead(BTN_4) == LOW;
  bool val8 = digitalRead(BTN_8) == LOW;
  if (val1 && val2 && val4 && val8) {
    return true;
  } else {
    return false;
  }
}

// Wipes highscore if all Buttons are pressed for 4 consecutive seconds
void checkReset() {
  if (checkResetButtons()) {
    digitalWrite(ledMap[0], HIGH);
    delay(1000);
    if (checkResetButtons()) {
      digitalWrite(ledMap[1], HIGH);
      delay(1000);
      if (checkResetButtons()) {
        digitalWrite(ledMap[2], HIGH);
        delay(1000);
        if (checkResetButtons()) {
          digitalWrite(ledMap[3], HIGH);
          delay(500);
          EEPROM.write(0, -1);
        }
      }
    }
  }
  resetLeds();
}

// loop is called indefinitly after `setup` is finished
void loop()
{
  // Send device to sleep after TIMEOUT_MS has passed without button interaction
  if (millis() - lastInteraction > TIMEOUT_MS) {
    sleep();
  }
  // if we reach the next level increase streak and check if we won
  if (counter > streak) {
    if(counter > 0){
      delay(500);
    }
    ++streak;
    if (streak == 15) {
      nightRiderEffect(100, 5);
      delay(300);
      gameOver(false);
    }
    sequence[streak] = char(random(0, 4));
    for (char i = 0; i <= streak; ++i) {
      flashLed(sequence[i]);
    }
    counter = 0;
    lastInteraction = millis();
  }

  // get any new button interactions and updated LEDs if necessary
  char btn = getButtonPressed();
  if (btn >= 0) {
    lastInteraction = millis();
    delay(100);
    // This ensures that accidental multiple button presses are ignored
    if (!waitForUnpress) {
      if (btn == sequence[counter]) {
        waitForUnpress = true;
      } else {
        gameOver(true);
      }
    }
  } else if (btn == -2) {
    lastInteraction = millis();
    delay(100);
    if (waitForUnpress) {
      counter++;
      waitForUnpress = false;
    }
  }
}

bool btn1Pressed = false;
bool btn2Pressed = false;
bool btn4Pressed = false;
bool btn8Pressed = false;

/*
  Returns either:
  + 0-3 when one button is newly pressed,
  + -1 when nothing happned or multiple buttons are currently pressed
  + -2 when the last button pressed has just been released
*/
char getButtonPressed() {
  // Read all Buttons
  bool val1 = digitalRead(BTN_1) == LOW;
  bool val2 = digitalRead(BTN_2) == LOW;
  bool val4 = digitalRead(BTN_4) == LOW;
  bool val8 = digitalRead(BTN_8) == LOW;

  bool stateChanged = false;

  // "if" away until all possible changes have been calculated
  if (val1 && !btn1Pressed) {
    digitalWrite(ledMap[0], HIGH);
    btn1Pressed = true;
    stateChanged = true;
  }
  if (!val1 && btn1Pressed) {
    digitalWrite(ledMap[0], LOW);
    btn1Pressed = false;
    stateChanged = true;
  }

  if (val2 && !btn2Pressed) {
    digitalWrite(ledMap[1], HIGH);
    btn2Pressed = true;
    stateChanged = true;
  }
  if (!val2 && btn2Pressed) {
    digitalWrite(ledMap[1], LOW);
    btn2Pressed = false;
    stateChanged = true;
  }

  if (val4 && !btn4Pressed) {
    digitalWrite(ledMap[2], HIGH);
    btn4Pressed = true;
    stateChanged = true;
  }
  if (!val4 && btn4Pressed) {
    digitalWrite(ledMap[2], LOW);
    btn4Pressed = false;
    stateChanged = true;
  }

  if (val8 && !btn8Pressed) {
    digitalWrite(ledMap[3], HIGH);
    btn8Pressed = true;
    stateChanged = true;
  }
  if (!val8 && btn8Pressed) {
    digitalWrite(ledMap[3], LOW);
    btn8Pressed = false;
    stateChanged = true;
  }

  // check which state Change actually occured, we want to ignore the case of multiple, simultanous button presses
  if (stateChanged) {
    if (btn1Pressed && !(btn2Pressed || btn4Pressed || btn8Pressed)) {
      return 0;
    }

    if (btn2Pressed && !(btn1Pressed || btn4Pressed || btn8Pressed)) {
      return 1;
    }

    if (btn4Pressed && !(btn1Pressed || btn2Pressed || btn8Pressed)) {
      return 2;
    }

    if (btn8Pressed && !(btn1Pressed || btn2Pressed || btn4Pressed)) {
      return 3;
    }

    if (!btn1Pressed && !btn2Pressed && !btn4Pressed && !btn8Pressed) {
      return -2;
    }
  }
  return -1;
}

// Flashes an LED for the specified time, will wait for the same amount afterwards
void flashLed(char led) {
  digitalWrite(ledMap[led], HIGH);
  delay(FLASH_TIME);
  digitalWrite(ledMap[led], LOW);
  delay(FLASH_TIME / 2);
}

// Sets all LEDS to LOW
void resetLeds() {
  for (char i = 0; i < 4; ++i) {
    digitalWrite(ledMap[i], LOW);
  }
}

// Sets the atmega chip to sleep mode, so that power consumption is minimized
void sleep() {
  resetLeds();
  sleep_enable();
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  sleep_cpu();
}

// displays a given score on the LEDs in binary format
void displayScore(char score, float frequency, char repetitions) {
  resetLeds();
  bool bits[] = {false, false, false, false};
  if (score > 15) {
    score = 15;
  }
  
  // find the bits of the score
  for (char i = 3; i >= 0; --i) {
    if (score % 2 == 1) {
      bits[i] = true;
    }
    score /= 2;
  }

  // show score in binary but pulse as a cos-wave
  float loopDuration = 1. / frequency;
  for (char r = 0; r < repetitions; ++r) {
    unsigned long startTime = millis();
    while (true) {
      float deltaSecs = ((float)(millis() - startTime)) / 1000.;
      if (deltaSecs > loopDuration) {
        break;
      }
      char cosVal = (char)((-1 * cos((deltaSecs / loopDuration) * 2 * PI)) * 128 + 128);
      for (char i = 0; i < 4; ++i) {
        if (bits[i]) {
          analogWrite(ledMap[i], cosVal);
        }
      }
    }
  }
  resetLeds();
}

// creates a quick swipe animation
void swipeLight(int animationSpeed, bool animationDirection) {

  if (animationDirection) {
    for (char i = 0; i < 4; ++i) {
      digitalWrite(ledMap[i], HIGH);
      delay(animationSpeed);
    }
    for (char i = 0; i < 4; ++i) {
      digitalWrite(ledMap[i], LOW);
      delay(animationSpeed);
    }
  } else {
    for (char i = 3; i >= 0; ++i) {
      digitalWrite(ledMap[i], HIGH);
      delay(animationSpeed);
    }
    for (char i = 3; i >= 0; ++i) {
      digitalWrite(ledMap[i], LOW);
      delay(animationSpeed);
    }
  }
}

// Makes a quick nightrider effect on the leds
void nightRiderEffect(int speed, int repetitions) {
  for (int r = 0; r < repetitions; ++r) {
    for (char i = 0; i < 6; ++i) {
      resetLeds();
      char led = i;
      if (i > 3) {
        led = 6 - i;
      }
      if (led == 0) {
        analogWrite(ledMap[led + 1], 64);
      } else if (led == 3) {
        analogWrite(ledMap[led - 1], 64);
      } else {
        analogWrite(ledMap[led - 1], 64);
        analogWrite(ledMap[led + 1], 64);
      }
      analogWrite(ledMap[led], 255);
      delay(speed);
    }
  }
}

// Display the Score and Loop indefinitly
void gameOver(bool isError) {
  if (highscore < streak) {
    EEPROM.write(0, streak);
  }

  if (isError) {
    for (char i = 0; i < 5; ++i) {
      digitalWrite(ledMap[0], HIGH);
      digitalWrite(ledMap[1], HIGH);
      digitalWrite(ledMap[2], HIGH);
      digitalWrite(ledMap[3], HIGH);
      delay(100);
      resetLeds();
      delay(100);
    }
  }

  displayScore(streak, 1., 7.);
  sleep();
}
