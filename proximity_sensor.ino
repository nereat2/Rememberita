const int ULTRA_PIN = 10; 

// defined in main.ino
extern int distance;
//extern const int ULTRA_PIN;

void readDistance() {
  pinMode(ULTRA_PIN, OUTPUT);
  digitalWrite(ULTRA_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(ULTRA_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(ULTRA_PIN, LOW);

  pinMode(ULTRA_PIN, INPUT);
  unsigned long us = pulseIn(ULTRA_PIN, HIGH, 40000UL);  // <-- use HIGH

  int cm = (us == 0) ? 150 : (int)(us / 58UL);
  if (cm > 510) cm = 150;

  distance = cm;
}
void ultrasonicOff() {
  pinMode(ULTRA_PIN, OUTPUT);
  digitalWrite(ULTRA_PIN, LOW);   
}


