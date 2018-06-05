void controlla_movimento();
void controlla_autenticazione();
void ottieni_coordinate();

bool loggato, in_movimento;
float posizioni[2];

void setup()
{
  bool loggato = false;
  bool in_movimento = false;
}

void loop()
{
  // uscita dal ciclo di polling solo in caso di situazione anomala
  // ovvero auto in movimento mentre l'utente non è loggato
  while(!in_movimento && loggato)
  {
    controlla_autenticazione();
    controlla_movimento();
  }

  // lettura coordinate gps dal sensore
  ottieni_coordinate();

  // stabilisci connessione

  // manda pacchetto
}

void controlla_autenticazione()
  {
    
  }