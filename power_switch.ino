// power_switch.ino

extern bool whiteActive;

// Pin definition lives in main .ino
extern const int POWER_SWITCH_PIN;

// Optional: track transitions
static bool lastPowerState = true;

void initPowerSwitch() {
  pinMode(POWER_SWITCH_PIN, INPUT_PULLUP);
}

// Returns true if device should be active
bool deviceIsOn() {
  // ON = switch connected to GND
  return digitalRead(POWER_SWITCH_PIN) == 0;
}

// Call this once per loop.
// Returns false if device is OFF and loop should stop.
bool handlePowerSwitch() {
  bool nowOn = deviceIsOn();

  // Just turned OFF
  if (!nowOn && lastPowerState) {
    whiteActive = false;
    neoOff();
    ultrasonicOff();
    // Optional: blank display here
    // clearDisplay();
  }

  // Just turned ON
  if (nowOn && !lastPowerState) {
    neoModeA();       // wake-up cue
    updateDisplay();
  }

  lastPowerState = nowOn;

  return nowOn;
}
