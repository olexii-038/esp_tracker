
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <time.h>
//#include <Arduino_JSON.h>
#include "secrets.h"

#define LED_PIN 2
#define LED_PERIOD 1000
#define RECONNECT_INTERVAL 10000

int dbInterval = 30000;

unsigned long statusTimer = 0;

unsigned long lastReconnect = 0;

unsigned long ledTimer = 0;
unsigned long lastToggle = 0;
static bool ledState = false;

WiFiCred networks[] = {
  {"Redmi Note 8 Pro", "a0f57ebfc2a8"},
  {"TOTOLINK_A3002RU_5G", "Emidot32"},
  {"TP-Link_34FA", "88912462"}
};

const int NETWORK_COUNT = sizeof(networks) / sizeof(networks[0]);

void setup() {
  // put your setup code here, to run once:
  pinMode(LED_PIN, OUTPUT);
  Serial.begin(115200);

  connectToWiFi();
}

void loop() {
  // put your main code here, to run repeatedly:
  launchLed();
  if (WiFi.status() != WL_CONNECTED) reconnectToWiFi();
  if (millis() - statusTimer > dbInterval) sendStatus();
}

void sendStatus() {
  statusTimer = millis();
  HTTPClient http;
  WiFiClientSecure client;

  client.setInsecure();

  http.begin(client, STATUS_URL);
  http.addHeader("Content-Type", "application/json");

  time_t now = time(nullptr);
  String text = String(now);
  struct tm t;
  localtime_r(&now, &t);
  String date = "\"" + String(t.tm_year + 1900) + "-" + String(t.tm_mon + 1) + "-" + String(t.tm_mday) + " " + String(t.tm_hour) + ":" + String(t.tm_min) + ":" + String(t.tm_sec) + "\"";
  String json = "{\"status\": \"online\", \"timestamp\": " + text + ", \"date\": " + date + "}";
  int code = http.PUT(json);
  dbInterval = code == 200 ? 30000 : 10000;
  Serial.println(code);

  /*
    JSONVar myObject;
    myObject["status"] = "online";
    myObject["timestamp"] = now;

    String jsonString = JSON.stringify(myObject);
  */

  http.end();
}

void launchLed() {
  ledTimer = millis();
    if (ledTimer - lastToggle >= LED_PERIOD / 2) {
      lastToggle = ledTimer;
      ledState = !ledState;
      digitalWrite(LED_PIN, ledState);
    }
}

void reconnectToWiFi() {
  if (millis() - lastReconnect > RECONNECT_INTERVAL) {
    Serial.println("Reconnecting");
    lastReconnect = millis();
    connectToWiFi();
  }
}


void connectToWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);
  Serial.println("Searching WiFi");
  int n = WiFi.scanNetworks();
  Serial.printf("Found %d networks\n", n);
  for(int i = 0; i < n; i++) {
    String foundSSID = WiFi.SSID(i);
    for(int j = 0; j < NETWORK_COUNT; j++) {
      if (foundSSID == networks[j].ssid) {
        Serial.print("Connecting to ");
        Serial.println(foundSSID);

        WiFi.begin(networks[j].ssid, networks[j].password);
        if (WiFi.waitForConnectResult() == WL_CONNECTED) {
          Serial.println("Connected!");
          Serial.print("IP: ");
          Serial.println(WiFi.localIP());
          setTime();
          return;
        }
      }
    }
  }
  Serial.println("No known networks found");
}

void setTime() {
  unsigned long startTime = millis();
  configTime(0, 0, "pool.ntp.org", "time.nist.gov");
  time_t now = time(nullptr);
  while (now < 1700000000 || now  > 10000000000) {
    delay(300);
    now = time(nullptr);
    Serial.println("Time: " + String(now));
    if (WiFi.status() != WL_CONNECTED) {
      Serial.println("WiFi lost: time is not synced!");
      return;
    }
    if (startTime - millis() > 15000) {
      Serial.println("Timeout  60s: time is not synced");
      return;
    }
  }
  Serial.print("Time synced: ");
  Serial.println(now);
}
