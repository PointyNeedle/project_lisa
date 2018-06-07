#include <EEPROM.h>

int addr = 0;
byte card[4] = {0x6F, 0x38, 0xF9, 0x28};
byte read_card[4];

void setup()
{
  Serial.begin(9600);
  EEPROM_read_card();
}

void loop()
{
  
}

void EEPROM_read_card()
{
  for (addr = 0; addr < 4; addr++)
  {
    read_card[addr] = EEPROM.read(addr);
    Serial.println(read_card[addr], HEX);
  }
  return;
}