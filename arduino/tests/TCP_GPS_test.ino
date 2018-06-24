#include "ESP8266.h"
#include "TinyGPS.h"

#define SSID ""
#define PASSWORD ""

TinyGPS gps;
SoftwareSerial esp8266(2, 3);
ESP8266 wifi(esp8266);
long lat, lon;
double latitude, longitude;
unsigned long fix_age;

void setup(void)
{
  Serial.begin(9600);
  Serial.print("setup begin\r\n");

  // aspetta finchè non ottieniamo una connessione all'hotspot
  //while (!wifi.joinAP(SSID, PASSWORD));

  Serial.print("setup end\r\n");
}

void loop(void)
{
  uint8_t buffer[1024];
  wifi.createTCP("192.168.43.1", 8000);

  uint32_t len = wifi.recv(buffer, sizeof(buffer), 10000);
  if (len > 0)
  {
    Serial.print("");
    for (uint32_t i = 0; i < len; i++)
    {
      if (gps.encode(((char)buffer[i])))
        gps.get_position(&lat, &lon, &fix_age);
      latitude = lat / 1000000.0;
      longitude = lon / 1000000.0;
      Serial.println(String(latitude, 6) + " " + String(longitude, 6));
    }
    Serial.print("\r\n");
  }
}
