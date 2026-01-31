#include "secrets.h"

const WiFiCred WIFI_LIST[] = {
  {"Ssid 1", "Password 1"},
  {"Ssid 2", "Password 2"}
};

const size_t NETWORK_COUNT = sizeof(WIFI_LIST) / sizeof(WIFI_LIST[0]);