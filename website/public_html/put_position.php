<?php
  if(isset($_POST['latitude']) && isset($_POST['longitude']) && isset($_POST['serial']))
    {
      include_once 'dbconnect.php';
      // ottieni dati dal form in POST
      $latitude = $_POST['latitude'];
      $longitude = $_POST['longitude'];
      $serial = $_POST['serial'];

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