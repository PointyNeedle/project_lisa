#include <EEPROM.h>
#include <ESP8266.h>
#include <I2Cdev.h>
#include <MFRC522.h>
#include <MPU6050.h>
#include <SPI.h>
#include <TinyGPS.h>
#include <Wire.h>

const byte NUMERO_SERIALE = 2;
const byte RST_PIN = 8;
const byte SS_PIN = 53;

void controlla_movimento();
void controlla_autenticazione();
void ottieni_coordinate();
void stabilisci_connessione();
String costruisci_richiesta();
void invia_richiesta();
//String costruisci_richiesta(char *host);
//void invia_richiesta(char *richiesta, int porta, char *host);
void leggi_UID_da_EEPROM(byte UID[]);
void leggi_UID_da_sensore(byte UID[]);
boolean controlla_byte_array(byte a[], byte b[]);

ESP8266 esp8266(Serial1);
MFRC522 mfrc522(SS_PIN, RST_PIN);
MPU6050 gy521;
TinyGPS gps;
boolean loggato, in_movimento;
float posizioni[2]; // [0] = latitudine, [1] = longitudine
String richiesta_String;
char richiesta[1024];

void setup()
  {
    // inizializzazione bus seriale, SPI e I2C
		Serial.begin(9600);
    Serial1.begin(9600);
    SPI.begin();
    Wire.begin();

    // inizializzazione accellerometro
    gy521.initialize();
    gy521.setFullScaleAccelRange(0x01); // intervallo impostato a ±4g

    //inizializzazione sensore RFID
    mfrc522.PCD_Init();
		
		Serial.println("Connessione all'AP...");
		esp8266.joinAP(/*SSID*/, /*password*/);
		Serial.println("Connessione!");

    loggato = false;
    in_movimento = false;
  }

void loop()
  {
    // uscita dal ciclo di polling solo in caso di situazione anomala
    // ovvero veicolo in movimento mentre l'utente non è loggato
    do
    {
			Serial.print("Loggato: " + String(loggato) + "\t");
			Serial.println("In movimento: " + String(in_movimento));
      controlla_autenticazione();
      controlla_movimento();
    }while(!(in_movimento && !loggato));
		Serial.println("Veicolo in movimento non autorizzato");
    // invia dati per 30 secondi, così da avere un numero 
    // considerevole di posizioni con il quale effettuare il 
    // tracciamento
    long numero_millisecondi_corrente = millis();
    while (millis() - numero_millisecondi_corrente < 1000)
      {
				Serial.println("Lettura coordinate dal sensore...");
        // lettura coordinate dal gps esterno
        ottieni_coordinate();
				
				//Serial.println("Stabilimento connessione...");
        // stabilisci connessione alla rete wifi, vista l'assenza 
        // dello shield GSM
        

        // assembla la richiesta HTTP da mandare all'API endpoint del server
        //richiesta_String = costruisci_richiesta("projectlisa.ml");
        //richiesta_String.toCharArray(richiesta, richiesta_String.length());
				
				//Serial.println(richiesta_String);
				
				
				Serial.println("Invio richiesta...");
        // invia richiesta
        //invia_richiesta(richiesta, 80, "projectlisa.ml");
				invia_richiesta();
      }
		in_movimento = false;
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
    const float soglia = 0.5;

    // lettura accellerazioni dal sensore
    gy521.getAcceleration(&ax, &ay, &az);

    // secondo il datasheet dell'MPU6050, con un intervallo
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
        while (millis() - numero_millisecondi_corrente < 100)
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
        
        // se il veicolo, dopo il calcolo delle medie, non risulta in movimento,
        // allora setta la flag di conseguenza
        if (!(axf > soglia || ayf > soglia || (azf - 0.9) > soglia))
          in_movimento = false;
      }
    return;
  }

void controlla_autenticazione()
  {
    byte EEPROM_UID[4] = {0x6F, 0x38, 0xF9, 0x28}; // UID contenuto nell'EEPROM dell'arduino
    byte card_UID[4] = {0x00, 0x00, 0x00, 0x00}; // UID letto dal sensore, inizializzato a 00 00 00 00

    // lettura degli UID
    //leggi_UID_da_EEPROM(EEPROM_UID);
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
    uint8_t buffer[1024];
    // connessione allo smarthphone con Share GPS in funzione
    esp8266.createTCP("192.168.43.1", 8000);

    // ricezione dati dal tunnel TCP
    uint32_t len = esp8266.recv(buffer, sizeof(buffer), 10000);

    // se ci sono dati in ricezione, effettua l'interpretazione 
    // delle sentences NMEA
    if(len > 0)
      {
        boolean gps_pronto;
        for (uint32_t i = 0; i < len; i++)
          {
            // diamo in pasto i dati ricevuti all'oggetto gps
            // la funzione ritorna true se l'interpretazione è andata 
            // a buon fine
            gps_pronto = gps.encode((char)buffer[i]);
            if(gps_pronto)
              {
                // variabili da usare per estrarre informazioni dall'oggetto GPS
                long lat, lon;
                unsigned long tempo_fix;

                // lettura coordinate
                gps.get_position(&lat, &lon, &tempo_fix);
                posizioni[0] = lat / 1000000.0;
                posizioni[1] = lon / 1000000.0;
              }
          }
				Serial.println("Longitudine: " + String(posizioni[0], 6) + "\t" + "Latitudine: " + String(posizioni[1], 6));
      }
    
    // chiusura connessione
    esp8266.releaseTCP();

    return;
  }

void stabilisci_connessione()
  {
    // ssid e password della rete wifi alla quale ci colleghiamo
    char *ssid = "";
    char *password = "";

    // aspetta finchè non ottieniamo una connessione all'access point
    while(!esp8266.joinAP(ssid, password));

    return;
  }

String costruisci_richiesta()
  {

    String richiesta_http;

    richiesta_http.concat("GET /post_position.php?");
    richiesta_http.concat("serial=2");
    richiesta_http.concat("&latitude=" + String(posizioni[0], 6));
    richiesta_http.concat("&longitude=" + String(posizioni[1], 6) + " ");
    richiesta_http.concat("HTTP/1.1\r\n");
    richiesta_http.concat("Host: " + String("projectlisa.ml") + "\r\n");
    richiesta_http.concat("Connection: close\r\n");
    richiesta_http.concat("\r\n\r\n");

    return richiesta_http;
  }	
	
/*String costruisci_richiesta(char *host)
  {

    String richiesta_http;

    richiesta_http.concat("GET /post_position.php?");
    richiesta_http.concat("serial=" + String(NUMERO_SERIALE));
    richiesta_http.concat("&latitude=" + String(posizioni[0], 6));
    richiesta_http.concat("&longitude=" + String(posizioni[1], 6) + " ");
    richiesta_http.concat("HTTP/1.1\r\n");
    richiesta_http.concat("Host: " + String(host) + "\r\n");
    richiesta_http.concat("Connection: close\r\n");
    richiesta_http.concat("\r\n\r\n");

    return richiesta_http;
  }*/

/*void invia_richiesta(char *richiesta, int porta, char *host)
  {
		uint8_t buffer[1024];
		
    // crea tunnel TCP, invia la richiesta, chiudi tunnel
    // non ci importano eventuali risposte, solo se siamo riusciti a mandare i dati
    esp8266.createTCP(host, porta);
    esp8266.send((const uint8_t *)richiesta, strlen(richiesta));
		
		uint32_t len = esp8266.recv(buffer, sizeof(buffer), 10000);
		if (len > 0)
			{
				for (uint32_t i = 0; i < len; i++)
				{
					Serial.print((char)buffer[i]);
				}
				Serial.print("\r\n");
			}
		
    esp8266.releaseTCP();
    return;
  }*/

void invia_richiesta()
	{
		String hello_string = costruisci_richiesta();
		uint8_t buffer[1024];
		char hello[1024];
		hello_string.toCharArray(hello, hello_string.length());
		Serial.print(hello);
		esp8266.createTCP("projectlisa.ml", 80);
		esp8266.send((const uint8_t *)hello, hello_string.length());
		uint32_t len = esp8266.recv(buffer, sizeof(buffer), 10000);
		if (len > 0)
		{
			Serial.print("Received:");
			for (uint32_t i = 0; i < len; i++)
			{
				Serial.print((char)buffer[i]);
			}
			Serial.print("\r\n");
		}
		Serial.print(hello_string);
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
            //Serial.println(UID[i], HEX);
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
