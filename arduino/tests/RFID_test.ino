#include <EEPROM.h> // We are going to read and write PICC's UIDs from/to EEPROM
#include <SPI.h>    // RC522 Module uses SPI protocol
#include <MFRC522.h>

#define RST_PIN 9 // Configurable, see typical pin layout above
#define SS_PIN 10 // Configurable, see typical pin layout above

MFRC522 mfrc522(SS_PIN, RST_PIN); // Create MFRC522 instance
byte readCard[4];                 // Stores scanned ID read from RFID Module
byte card[4] = {0x6F, 0x38, 0xF9, 0x28};

void setup()
{
  Serial.begin(9600);                // Initialize serial communications with the PC
  while (!Serial);                   // Do nothing if no serial port is opened (added for Arduinos based on ATMEGA32U4)
  SPI.begin();                       // Init SPI bus
  mfrc522.PCD_Init();                // Init MFRC522
}

void loop()
{
  getID();
  if (checkTwo(readCard, card))
    Serial.println("KEK");
  readCard[0] = 0x00;
}

uint8_t getID() {
  // Getting ready for Reading PICCs
  if ( ! mfrc522.PICC_IsNewCardPresent()) { //If a new PICC placed to RFID reader continue
    return 0;
  }
  if ( ! mfrc522.PICC_ReadCardSerial()) {   //Since a PICC placed get Serial and continue
    return 0;
  }
  // There are Mifare PICCs which have 4 byte or 7 byte UID care if you use 7 byte PICC
  // I think we should assume every PICC as they have 4 byte UID
  // Until we support 7 byte PICCs
  Serial.println(F("Scanned PICC's UID:"));
  for ( uint8_t i = 0; i < 4; i++) {  //
    readCard[i] = mfrc522.uid.uidByte[i];
    Serial.print(readCard[i], HEX);
  }
  Serial.println("");
  mfrc522.PICC_HaltA(); // Stop reading
  return 1;
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
