SET time_zone = "+02:00";

CREATE TABLE IF NOT EXISTS users (
  id INTEGER NOT NULL AUTO_INCREMENT,
  username varchar(100) NOT NULL,
  email varchar(100) NOT NULL,
  password varchar(255) NOT NULL,
  codice_LISA INTEGER,
  PRIMARY KEY (id),
  FOREIGN KEY (codice_LISA) REFERENCES LISAs(codice_seriale)
);

CREATE TABLE IF NOT EXISTS LISAs (
  codice_seriale INTEGER NOT NULL AUTO_INCREMENT,
  modello CHAR(4),
  PRIMARY KEY (codice_seriale)
);

CREATE TABLE IF NOT EXISTS posizioni (
  id INTEGER NOT NULL AUTO_INCREMENT,
  latitudine DECIMAL(9,6),
  longitudine DECIMAL(9,6),
  data_ora DATETIME,
  codice_LISA INTEGER,
  PRIMARY KEY (id),
  FOREIGN KEY (codice_LISA) REFERENCES LISAs(codice_seriale)
)