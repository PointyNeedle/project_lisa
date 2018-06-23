#include "ESP8266.h"

#define SSID        ""
#define PASSWORD    ""

SoftwareSerial esp8266(2,3);
ESP8266 wifi(esp8266);

void setup(void)
{
    Serial.begin(9600);
    Serial.print("setup begin\r\n");

    // aspetta finchè non ottieniamo una connessione all'hotspot
    while(!wifi.joinAP(SSID, PASSWORD));

    Serial.print("setup end\r\n");

}

void loop(void)
{
    uint8_t buffer[1024];
    wifi.createTCP("api.ipify.org", 80);

    char *hello = "GET / HTTP/1.1\r\nHost: api.ipify.org\r\nConnection: close\r\n\r\n";
    wifi.send((const uint8_t*)hello, strlen(hello));

    uint32_t len = wifi.recv(buffer, sizeof(buffer), 10000);
    if (len > 0) {
        Serial.print("Received:");
        for(uint32_t i = 0; i < len; i++) {
            Serial.print((char)buffer[i]);
        }
        Serial.print("\r\n");
    }
}
