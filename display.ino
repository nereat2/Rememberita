#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 128
#define OLED_ADDR 0x3C //or 0x3D, it depends

Adafruit_SH1107 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire);

//Items list
extern const uint8_t epd_bitmap_keys[];
extern const uint8_t epd_bitmap_wallet[];
extern const uint8_t epd_bitmap_headphones[];
extern const uint8_t epd_bitmap_charger[];
extern const uint8_t epd_bitmap_laptop[];
extern const uint8_t epd_bitmap_lunch[];
extern const uint8_t epd_bitmap_medicine[];
extern const uint8_t epd_bitmap_phone[];
extern const uint8_t epd_bitmap_stove[];
extern const uint8_t epd_bitmap_gift[];


void initDisplay() {
  Wire.begin(); 

  if(!display.begin(OLED_ADDR, true)) {
    Serial.println("OLED not found. Check wiring/address");
    while (true) { delay(10); }
  }

  display.clearDisplay();
  display.setTextColor(SH110X_WHITE);
  display.setTextWrap(true);

  //FIX THIS
  display.setTextSize(3);
  display.setCursor(0, 0); //change this to the center !!
  display.println("Rememberita ready!");
  display.display();
}


void drawIcon(const uint8_t* bmp) {
  display.drawBitmap(0, 0, bmp, 128, 128, SH110X_WHITE);
  display.display();
}

void updateDisplay() {
  display.clearDisplay();
  display.setCursor(0, 0);
  display.setTextWrap(false);
  display.setTextColor(SH110X_WHITE);

  bool recognized = false;

  if (taskActive) {
    String t = currentTask;
    t.trim();
    t.toLowerCase();

    if (t == "keys" || t == "key")            { drawIcon(epd_bitmap_keys); return; }
    if (t == "wallet")                        { drawIcon(epd_bitmap_wallet); return; }
    if (t == "headphones")                    { drawIcon(epd_bitmap_headphones); return; }
    if (t == "charger")                       { drawIcon(epd_bitmap_charger); return; }
    if (t == "laptop" || t == "pc")           { drawIcon(epd_bitmap_laptop); return; }
    if (t == "lunch" || t == "lunchbox")      { drawIcon(epd_bitmap_lunch); return; }
    if (t == "medicine" || t == "pill")       { drawIcon(epd_bitmap_medicine); return; }
    if (t == "phone")                         { drawIcon(epd_bitmap_phone); return; }
    if (t == "stove")                         { drawIcon(epd_bitmap_stove); return; }
    if (t == "gift")                         { drawIcon(epd_bitmap_gift); return; }

    if (t.length() > 12) t = t.substring(0, 12);
    display.println(t);
  }

  if (!recognized) {
    // TEXT FALLBACK (this is the part that was “falling”)
    display.clearDisplay();
    display.setTextColor(SH110X_WHITE);

    String taskUpper = currentTask;
    taskUpper.toUpperCase();

    drawBigCenteredTextSingleLine(taskUpper);
  }

  display.display();
}

void drawBigCenteredTextSingleLine(const String &text) {
  int16_t x1, y1;
  uint16_t w, h;

  for (int size = 6; size >= 1; size--) {
    display.setTextSize(size);
    display.getTextBounds(text, 0, 0, &x1, &y1, &w, &h);

    // MUST fit horizontally
    if (w <= 128) {
      int16_t x = (128 - w) / 2;
      int16_t y = (128 - h) / 2;

      display.setCursor(x, y);
      display.print(text);
      return;
    }
  }
}




