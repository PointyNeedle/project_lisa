#include <SoftwareSerial.h>

//Schema collegamento
// Arduino | ESP8266
//    2(RX)| TX
//    3(TX)| RX

//SoftwareSerial Serial1(2, 3); // RX, TX

void setup()

{
  uint32_t baud = 9600;
  Serial.begin(baud);
  Serial1.begin(baud);
  Serial.print("SETUP!! @");
  Serial.println(baud);
}

void loop()
{
  while (Serial1.available() > 0)
  {
    char a = Serial1.read();
    if (a == '\0')
      continue;
    if (a != '\r' && a != '\n' && (a < 32))
      continue;
    Serial.print(a);
  }

  while (Serial.available() > 0)
  {
    char a = Serial.read();
    Serial.write(a);
    Serial1.write(a);
  }
}