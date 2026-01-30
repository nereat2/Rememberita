extern const int BUTTON_PIN;
extern const int BUTTON2_PIN;
extern bool whiteActive;
extern unsigned long whiteStartTime;

int buttonVal = 0;
int oldButtonVal = 0;

void initButton() {
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(BUTTON2_PIN, INPUT_PULLUP);

  int b1 = digitalRead(BUTTON_PIN);
  int b2 = digitalRead(BUTTON2_PIN);
  buttonVal = (b1 == 1 || b2 == 1) ? 1 : 0;
  oldButtonVal = buttonVal;
}

void checkButton() {
  int b1 = digitalRead(BUTTON_PIN);
  int b2 = digitalRead(BUTTON2_PIN);

  // pressed if either button reads 1
  buttonVal = (b1 == 1 || b2 == 1) ? 1 : 0;

  // detect press (0 -> 1)
  if (oldButtonVal == 0 && buttonVal == 1) {
    whiteActive = true;
    whiteStartTime = millis();
  }

  oldButtonVal = buttonVal;
}
