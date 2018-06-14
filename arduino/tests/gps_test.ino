#include <TinyGPS++.h>
#include <SoftwareSerial.h>

SoftwareSerial ss(4, 3);
TinyGPSPlus gps;
float posizioni[2]; // [0] = latitudine, [1] = longitudine

void setup
  {
    Serial.begin(9600);
    ss.begin(9600);
  }

void loop
  {
    while (ss.available() > 0)
      gps.encode(ss.read());
    if (gps.location.isUpdated())
      {
        posizioni[0] = gps.location.lat();
        posizioni[1] = gps.location.lng();
      }
  }