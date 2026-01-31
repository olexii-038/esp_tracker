#pragma once

#define DB_URL "https://your-database-url.firebasedatabase.app/"
#define STATUS_URL "https://your-database-url.firebasedatabase.app/devices/ESP-1.json"


struct WiFiCred {
  const char* ssid;
  const char* password;
};

extern const WiFiCred WIFI_LIST[];
extern const size_t NETWORK_COUNT;
