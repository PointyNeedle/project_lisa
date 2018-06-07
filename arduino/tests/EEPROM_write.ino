#include <EEPROM.h>

int addr = 0;
byte card[4] = {0x6F, 0x38, 0xF9, 0x28};
byte read_card[4];

void setup()
  {
    Serial.begin(9600);
  }

void loop()
  {
    EEPROM_read_card();

    if (!checkTwo(card, read_card))
    {
      for (addr = 0; addr < 4; addr++)
      {
        EEPROM.write(addr, card[addr]);
        Serial.println(card[addr], HEX);
      }
    }
    else
      {
        Serial.println("DONE");
      }
  }

void EEPROM_read_card()
  {
    for (addr = 0; addr < 4; addr++)
      {
        read_card[addr] = EEPROM.read(addr);
      }
    return;
  }

boolean checkTwo(byte a[], byte b[])
{
  boolean match;
  if (a[0] != 0)  // Make sure there is something in the array first
    match = true; // Assume they match at first
  for (uint8_t k = 0; k < 4; k++)
  {                   // Loop 4 times
    if (a[k] != b[k]) // IF a != b then set match = false, one fails, all fail
      match = false;
  }
  if (match)
  {              // Check to see if if match is still true
    return true; // Return true
  }
  else
  {
    return false; // Return false
  }
}
