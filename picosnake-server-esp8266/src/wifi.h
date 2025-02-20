#include <ESP8266WiFi.h>

#ifndef WIFI_H
#define WIFI_H

#ifdef __cplusplus
extern "C" {
#endif

#define WIFI_MODE_WIFI_AP 1
#define WIFI_MODE_WIFI_STA 1

extern const char* ssid;
extern const char* password;
extern const char* ap_ssid;
extern const char* ap_password;

void wifi_setup();
void wifi_main();

#ifdef __cplusplus
}
#endif

#endif
