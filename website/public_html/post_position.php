<?php
  if(isset($_GET['latitude']) && isset($_GET['longitude']) && isset($_GET['serial']))
    {
      include_once 'dbconnect.php';
      // ottieni dati dal form in GET
      $latitude = $_GET['latitude'];
      $longitude = $_GET['longitude'];
      $serial = $_GET['serial'];

      // controllo presenza dispositivo nel database
      $risultato = $conn->query("SELECT * FROM users WHERE codice_LISA=" . $serial);
      if($risultato->num_rows == 1)
        {
          $stmt = $conn->prepare("INSERT INTO posizioni VALUES(NULL, ?, ?, NOW(), ?)");
          $stmt->bind_param("ddi", $latitude, $longitude, $serial);
          $stmt->execute();
          echo("$latitude, $longitude, $serial");
        }
      else
        echo("serial doesn't exist");
    }
  else
    {
      echo("no data");
    }
?>