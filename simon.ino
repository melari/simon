#define PURPLE_LED 12
#define GREEN_LED 9
#define PINK_LED A0
#define ORANGE_LED A3
#define PURPLE_BTN 11
#define GREEN_BTN 6
#define PINK_BTN A2
#define ORANGE_BTN A4
#define BEEP 10
#define NOISE A5

#define C 261.64
#define D 293.68
#define E 329.64
#define F 349.24
#define G 392.00
#define A 440
#define B 493.92
#define HIGH_C 523.28

#define WAITING 0
#define SHOWING 1
#define WATCHING 2

int lights[4] = {PURPLE_LED, GREEN_LED, PINK_LED, ORANGE_LED, };
int buttons[4] = {PURPLE_BTN, GREEN_BTN, PINK_BTN, ORANGE_BTN };
int sounds[4] = { C, E, G, HIGH_C };

int gameState = WAITING;
int level = 1;
int pattern[30];

unsigned long lastBeep = 0;
unsigned long timer = 0;

void flashAndTone(int i, int length = 200) {
  digitalWrite(lights[i], HIGH);
  tone(BEEP, sounds[i]);
  delay(length);
  noTone(BEEP);
  digitalWrite(lights[i], LOW);
}

unsigned long entropy() {
  return millis()
    + analogRead(5)
    + (analogRead(13) * 2)
    + (analogRead(A1) * 4)
    + (analogRead(A5) * 8);
}

void setup() {
  for (int pin : lights) {
    pinMode(pin, OUTPUT);
  }
  for (int pin : buttons) {
    pinMode(pin, INPUT_PULLUP);
  }
  pinMode(BEEP, OUTPUT);
  
  for (int i=0; i < 4; i++) {
    flashAndTone(i, 100);
  }
  transitionToWaitingState();
}

void transitionToWaitingState() {
  gameState = WAITING;
  lastBeep = millis();
  delay(1000);
}

void loopWaiting() {
  if (millis() - lastBeep > 10000) {
    lastBeep = millis();
    tone(BEEP, HIGH_C);
    delay(200);
    noTone(BEEP);
  }

  for (int i=0; i < 4; i++) {
    digitalWrite(lights[i], HIGH);
    if (digitalRead(buttons[i]) == LOW) {
      startGame();
      break;
    }
  }
}

void startGame() {
  randomSeed(entropy());
  for (int i=0; i < 4; i++) {
    digitalWrite(lights[i], LOW);
  }

  tone(BEEP, C);
  delay(100);
  tone(BEEP, D);
  delay(100);
  noTone(BEEP);

  gameState = SHOWING;
  level = 1;
  for(int i = 0; i <30; i++) {
    pattern[i] = random(4);
  }

  delay(1000);
}

void gameOver() {
  for (int i = 3; i >= 0; i--) {
    flashAndTone(i);
  }
  transitionToWaitingState();
}

void loopShowing() {
  for (int i = 0; i < level; i++) {
    flashAndTone(pattern[i]);
    delay(max(250, 500 - 25*level));
  }
  gameState = WATCHING;
}

void loopWatching() {
  int found = false;
  for (int i = 0; i < level; i++) {
    timer = millis();
    found = false;
    while(!found) {
      if (millis() - timer > 10000) {
        gameOver();
        return;
      }
      for (int j = 0; j < 4; j++) {
        if (digitalRead(buttons[j]) == LOW) {
          delay(50); // DEBOUNCE
          // wait for them to release the button
          while(digitalRead(buttons[j]) == LOW) { 
            digitalWrite(lights[j], HIGH);
          }
          flashAndTone(j);
          if (pattern[i] != j) {
            gameOver();
            return;
          } else {
            found = true;
            break;
          }
        }
      }
    }
  }

  delay(1000);
  level++;
  gameState = SHOWING;
}

void loop() {
  if (gameState == WAITING) { loopWaiting(); }
  if (gameState == SHOWING) { loopShowing(); }
  if (gameState == WATCHING) { loopWatching(); }
}
