#include <EEPROM.h>
#include <SPI.h> 
#include <MFRC522.h>

#define RST_PIN 9 // Configurable, see typical pin layout above
#define SS_PIN 10 // Configurable, see typical pin layout above

void controlla_movimento();
void controlla_autenticazione();
void ottieni_coordinate();
void leggi_UID_da_EEPROM(byte UID[]);
void leggi_UID_da_sensore(byte UID[]);


MFRC522 mfrc522(SS_PIN, RST_PIN);
boolean loggato, in_movimento;
float posizioni[2];

void setup()
  {
    Serial.begin(9600);
    SPI.begin();
    mfrc522.PCD_Init();
    boolean loggato = false;
    boolean in_movimento = false;
  }

void loop()
  {
    // uscita dal ciclo di polling solo in caso di situazione anomala
    // ovvero auto in movimento mentre l'utente non è loggato
    do
    {
      controlla_autenticazione();
      controlla_movimento();
    }while(!(in_movimento && !loggato));
    
    // lettura coordinate gps dal sensore
    ottieni_coordinate();

    // stabilisci connessione

    // manda pacchetto

  }

void controlla_movimento()
  {
    return;
  }

void controlla_autenticazione()
{
  byte EEPROM_UID[4]; // UID contenuto nell'EEPROM dell'arduino
  byte card_UID[4] = {0x00, 0x00, 0x00, 0x00}; // UID letto dal sensore, inizializzato a 00 00 00 00

  // lettura degli UID
  leggi_UID_da_EEPROM(EEPROM_UID);
  leggi_UID_da_sensore(card_UID);

  // controllo se i due UID corrispondono
  boolean corrisponde = controlla_byte_array(EEPROM_UID, card_UID);

  // se corrispondono, autentica o deautentica a seconda del 
  // valore di loggato
  if(corrisponde)
    loggato = !loggato;
  return;
}

void ottieni_coordinate()
  {
    return;
  }

void leggi_UID_da_EEPROM(byte UID[])
  {
    for (int addr = 0; addr < 4; addr++)
      UID[addr] = EEPROM.read(addr);
    return;
  }

void leggi_UID_da_sensore(byte UID[])
  {
    // se il sensore è pronto, effettua la lettura
    if(mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial())
      {
        for(int i = 0; i < 4; i++)
          {
            UID[i] = mfrc522.uid.uidByte[i];
            Serial.println(UID[i], HEX);
          }
      }
    mfrc522.PICC_HaltA(); // ferma la lettura
    return;
  }

boolean controlla_byte_array(byte a[], byte b[])
{
  boolean match = true;
  for (int k = 0; k < 4; k++)
  {                   
    if (a[k] != b[k]) 
      match = false;
  }
  return match;
}