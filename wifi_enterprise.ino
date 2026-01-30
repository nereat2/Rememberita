#ifdef USE_ENTERPRISE_WIFI

#include "esp_wpa2.h"

// WiFi
const char* WIFI_SSID = "SUPSI";
const char* EAP_IDENTITY = "nerea.asensio@supsi.ch";
const char* EAP_USERNAME = "nerea.asensio@supsi.ch";
const char* EAP_PASSWORD = "Interaction_2025!";

void connectEnterpriseWiFi() {
  WiFi.disconnect(true);
  delay(200);

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID);

  esp_wifi_sta_wpa2_ent_set_identity(
    (uint8_t*)EAP_IDENTITY, strlen(EAP_IDENTITY)
  );
  esp_wifi_sta_wpa2_ent_set_username(
    (uint8_t*)EAP_USERNAME, strlen(EAP_USERNAME)
  );
  esp_wifi_sta_wpa2_ent_set_password(
    (uint8_t*)EAP_PASSWORD, strlen(EAP_PASSWORD)
  );

  esp_wifi_sta_wpa2_ent_enable();
  Serial.print("Connecting to university WiFi");

  unsigned long startAttemptTime = millis();
  while (WiFi.status() != WL_CONNECTED &&
    millis() - startAttemptTime < 20000) {
    delay(500);
    Serial.print(".");
    }
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWiFi connected!");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("\nFailed to connect to SUPSI WIFI");
  }

} 
#endif
