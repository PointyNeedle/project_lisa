#include <SoftwareSerial.h>
#include <SerialESP8266wifi.h>

//Schema collegamento
// Arduino | ESP8266
//    2(RX)| TX
//    3(TX)| RX

SoftwareSerial softSerial(2, 3); // RX, TX
SerialESP8266wifi wifi(softSerial, softSerial, 5);

void setup()

{
  uint32_t baud = 9600;
  Serial.begin(baud);
  wifi.begin();
  wifi.connectToAP("z1ap", "ayylmao1");
  wifi.connectToServer("api.ipify.org", "80");
  wifi.send(SEVER, "GET / HTTP1.1\r\nHost: api.ipify.org\r\nConnection: close\r\n\r\n");
}

void loop()
{
  
}