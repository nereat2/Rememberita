/* #ifdef USE_HOME_WIFI

// Simple WPA2-Personal WiFi (home / hotspot)

const char* WIFI_SSID = "CASA-HOBBIT";
const char* WIFI_PASS = "gonellabianco2018";

void connectHomeWifi() {
  WiFi.disconnect(true);
  delay(200);

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);

  Serial.print("Connecting to home WiFi");

  unsigned long startAttemptTime = millis();
  while (WiFi.status() != WL_CONNECTED &&
         millis() - startAttemptTime < 15000) {
    delay(500);
    Serial.print(".");
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWiFi connected!");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("\nFailed to connect to home WiFi.");
  }
}
#endif */
