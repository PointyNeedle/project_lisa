#include <EEPROM.h>
#include <I2Cdev.h>
#include <MFRC522.h>
#include <MPU6050.h>
#include <SPI.h> 
#include <Wire.h>

#define RST_PIN 9 
#define SS_PIN 10 

void controlla_movimento();
void controlla_autenticazione();
void ottieni_coordinate();
void leggi_UID_da_EEPROM(byte UID[]);
void leggi_UID_da_sensore(byte UID[]);
boolean controlla_byte_array(byte a[], byte b[]);

MFRC522 mfrc522(SS_PIN, RST_PIN);
MPU6050 gy521;
boolean loggato, in_movimento;
float posizioni[2];

void setup()
  {
    // inizializzazione bus seriale, SPI e I2C
    Serial.begin(9600);
    SPI.begin();
    Wire.begin();

    // inizializzazione accellerometro
    gy521.initialize();
    gy521.setFullScaleAccelRange(0x01); // sensibilità impostata a ±4g

    //inizializzazione sensore RFID
    mfrc522.PCD_Init();

    loggato = false;
    in_movimento = false;
  }

void loop()
  {
    // uscita dal ciclo di polling solo in caso di situazione anomala
    // ovvero veicolo in movimento mentre l'utente non è loggato
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
    // accellerazioni raw rilevate sui 3 assi 
    int16_t ax, ay, az;
    // somma delle accellerazioni per asse
    long somma_ax = 0, somma_ay = 0, somma_az = 0;
    // accellerazioni convertite in g (1g = 9.81 m/s^2)
    float axf, ayf, azf;

    // la soglia di rilevazione del movimento è impostata a 1.2g (11.7 m/s^2)
    // abbastanza da capire se il veicolo si trova in movimento
    const float soglia = 1.2;

    // lettura accellerazioni dal sensore
    gy521.getAcceleration(&ax, &ay, &az);

    // secondo il datasheet dell'MPU6050, con una sensibilità
    // di ±4g il valore ottenuto deve essere diviso per 8192

    // conversione dati raw in g
    axf = ax / 8192.0;
    ayf = ay / 8192.0;
    azf = az / 8192.0;

    // primo controllo se il veicolo è in movimento
    if(abs(axf) > soglia || abs(ayf) > soglia || (abs(azf) - 0.9) /* correzione per la forza di gravità*/ > soglia)
      {
        in_movimento = true;

        // controllo se il veicolo rimane in movimento per più di 1.5
        // secondi, in modo da filtrare eventi come la chiusura di
        // uno sportello oppure l'entrata del conducente o di un
        // passeggero

        // il controllo viene effettuato calcolando le accellerazioni medie
        // nell'arco di 1.5 secondi
        int numero_misurazioni = 0;
        long numero_millisecondi_corrente = millis();
        while (millis() - numero_millisecondi_corrente < 1500)
          {
            gy521.getAcceleration(&ax, &ay, &az);
            numero_misurazioni++;
            somma_ax += abs(ax);
            somma_ay += abs(ay);
            somma_az += abs(az);
          }
        // calcolo delle medie e conversione immediata in g
        axf = (somma_ax / numero_misurazioni) / 8192.0;
        ayf = (somma_ay / numero_misurazioni) / 8192.0;
        azf = (somma_az / numero_misurazioni) / 8192.0;

        if (!(axf > soglia || ayf > soglia || (azf - 0.9) > soglia))
          in_movimento = false;
      }
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