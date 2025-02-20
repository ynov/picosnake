#include <ESP8266WiFi.h>

#include "wifi.h"
#include "page.h"

WiFiServer server(80);

void wifi_setup()
{
#if WIFI_MODE_WIFI_AP && WIFI_MODE_WIFI_STA
    WiFi.mode(WIFI_AP_STA);

    WiFi.softAP(ap_ssid, ap_password);
    Serial.printf("log: access point ssid: %s\r\n", ap_ssid);
    Serial.printf("log: ap ip_address: %s\r\n", WiFi.softAPIP().toString().c_str());

    WiFi.begin(ssid, password);
    Serial.printf("log: connecting to wifi\r\n");
    while (WiFi.status() != WL_CONNECTED) {
        delay(100);
    }
    Serial.printf("log: connected\r\n");
    Serial.printf("log: sta ip_address: %s\r\n", WiFi.localIP().toString().c_str());
#elif WIFI_MODE_WIFI_AP
    WiFi.mode(WIFI_AP);
    WiFi.softAP(ap_ssid, ap_password);
    Serial.printf("log: access point ssid: %s\r\n", ap_ssid);
    Serial.printf("log: ip_address: %s\r\n", WiFi.softAPIP().toString().c_str());
#else
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);

    Serial.printf("log: connecting to wifi\r\n");
    while (WiFi.status() != WL_CONNECTED) {
        delay(100);
    }

    Serial.printf("log: connected\r\n");
    Serial.printf("log: ip_address: %s\r\n", WiFi.localIP().toString().c_str());
#endif

    server.begin();
}

void wifi_main()
{
    WiFiClient client = server.accept();

    if (client) {
        String currentLine = "";
        String commandName = "";
        bool commandProcessed = false;

        while (client.connected()) {
            if (client.available()) {
                char c = client.read();

                if (c == '\n') {
                    if (!commandProcessed && currentLine.startsWith("GET")) {
                        if (currentLine.startsWith("GET /up")) {
                            commandName = "move_up";
                            commandProcessed = true;
                            Serial.println("cmd:move_up");
                        } else if (currentLine.startsWith("GET /left")) {
                            commandName = "move_left";
                            commandProcessed = true;
                            Serial.println("cmd:move_left");
                        } else if (currentLine.startsWith("GET /right")) {
                            commandName = "move_right";
                            commandProcessed = true;
                            Serial.println("cmd:move_right");
                        } else if (currentLine.startsWith("GET /down")) {
                            commandName = "move_down";
                            commandProcessed = true;
                            Serial.println("cmd:move_down");
                        }
                    }

                    if (currentLine.length() == 0) {
                        client.println("HTTP/1.1 200 OK");
                        client.println("Content-type:text/html");
                        client.println();

                        if (commandProcessed) {
                            client.printf("%s OK.\r\n", commandName.c_str());
                        } else {
                            client.println(PageContent);
                        }
                        break;
                    }
                    currentLine = "";
                } else if (c != '\r') {
                    currentLine += c;
                }
            }
        }

        client.stop();
    }
}
