#include <SoftwareSerial.h>
#include <ESP8266.h>

//Schema collegamento
// Arduino | ESP8266
//    2(RX)| TX
//    3(TX)| RX

SoftwareSerial esp8266(2, 3); // RX, TX
ESP8266 wifi(esp8266);
int baud = 9600;

void setup()
  {
    Serial.begin(baud);
    esp8266.begin(baud);
    wifi.setOprToStation();
    if(wifi.joinAP("z1ap", "ayylmao1"))
      {
        Serial.println("daje annemo\r");
      }
  }

void loop()
  {
    uint8_t buffer[1024];
    wifi.createTCP("api.ipify.org", "80");
    char request[] = "GET / HTTP1.1\r\nHost: api.ipify.org\r\nConnection: close\r\n\r\n";
    wifi.send(request[], strlen(request));
    uint32_t len = wifi.recv(buffer, sizeof(buffer), 10000);
    if (len > 0)
    {
      Serial.print("Received:[");
      for (uint32_t i = 0; i < len; i++)
      {
        Serial.print((char)buffer[i]);
      }
      Serial.print("]\r\n");
    }
  }