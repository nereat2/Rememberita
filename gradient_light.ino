#include <Adafruit_NeoPixel.h>

// Neopixel
const int PIN_PIXELS  = 6;     // D3
const int PIXEL_COUNT = 24;    // 

Adafruit_NeoPixel pixels(PIXEL_COUNT, PIN_PIXELS, NEO_GRB + NEO_KHZ800);

// Mode B animation state
int modeB_offset = 0;
int modeB_delayMs = 80;   // slower rotation
unsigned long modeB_lastMs = 0;


void neoBegin() {
  pixels.begin();
  pixels.clear();
  pixels.show();
}

void neoModeA() {  // warm yellow
  pixels.clear();
  for (int i = 0; i < PIXEL_COUNT; i++) {
    pixels.setPixelColor(i, pixels.Color(255, 140, 20));
  }
  pixels.show();
}

void neoModeB() { // spinning violet → blue gradient

  int d = distance;
  if (d < 10) d = 10;
  if (d > 50) d = 50;
  if (d <= 15) modeB_delayMs = 1;          // insane fast when very close
  else modeB_delayMs = map(d, 15, 50, 8, 140);

  unsigned long now = millis();
  unsigned long elapsed = now - modeB_lastMs;
  if (elapsed < (unsigned long)modeB_delayMs) return;

  int steps = elapsed / (unsigned long)modeB_delayMs;   // how many frames we missed
  if (steps > 5) steps = 5; // cap so it doesn't jump too far after long blocks
  modeB_lastMs += (unsigned long)steps * (unsigned long)modeB_delayMs;


  for (int i = 0; i < PIXEL_COUNT; i++) {
    int pos = (i + modeB_offset) % PIXEL_COUNT;
    float t = (float)pos / (PIXEL_COUNT - 1);

    uint8_t r = (uint8_t)((1.0f - t) * 190.0f);
    uint8_t g = 0;
    uint8_t b = 255;

    pixels.setPixelColor(i, pixels.Color(r, g, b));
  }

  pixels.show();

  int baseStep = (d <= 15) ? 2 : 1;              // close -> faster
  modeB_offset = (modeB_offset + baseStep * steps) % PIXEL_COUNT;

}

void neoModeC() { // white flash then off
  for (int i = 0; i < PIXEL_COUNT; i++) {
    pixels.setPixelColor(i, pixels.Color(255, 255, 255));
  }
  pixels.show();
}

void neoOff() {
  pixels.clear();
  pixels.show();
}
