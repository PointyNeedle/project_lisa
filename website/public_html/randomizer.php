<?php
  if(isset($_POST['serial']))
    {
      include_once 'dbconnect.php';
      // ottieni dati dal form in POST
      $serial = $_POST['serial'];

      // controllo presenza dispositivo nel database
      $risultato = $conn->query("SELECT * FROM users WHERE codice_LISA=" . $serial);
      if($risultato->num_rows == 1)
        {
          for($i = 0; $i < 10; $i++)
            {
              // range= -90, +90
              $latitude = rand(-90, 90);
              // range= -180, +180
              $longitude = rand(-180, 180);          
              $stmt = $conn->prepare("INSERT INTO posizioni VALUES(NULL, ?, ?, NOW(), ?)");
              $stmt->bind_param("ddi", $latitude, $longitude, $serial);
              $stmt->execute();
              echo("$latitude, $longitude, $serial");
            }
        }
      else
        echo("serial doesn't exist");
    }
  else
    {
      echo("no data");
    }
?>