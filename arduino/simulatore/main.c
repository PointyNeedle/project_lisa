#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>


void init();
void controlla_movimento();
void controlla_autenticazione();
void ottieni_coordinate();

bool loggato, in_movimento;
float posizioni[2]; // [0] = latitudine, [1] = longitudine

int main(void)
  {
    // setup()
    init();

    // loop()
    while(true)
      {
        // uscita dal ciclo di polling solo in caso di situazione anomala
        // (auto in movimento mentre l'utente non è loggato)
        while (!in_movimento && loggato)
        {
          controlla_autenticazione();
          controlla_movimento();
        }

        // lettura coordinate gps dal sensore
        ottieni_coordinate();

        // stabilisci connessione
        
        // manda pacchetto
      }
    return 0;
  }

void init()
  {
    loggato = false;
    in_movimento = false;
  }

