<?php
ob_start();
session_start();
require_once 'dbconnect.php';

if (!isset($_SESSION['user'])) {
    header("Location: login.php");
    exit;
}

// l'utente vuole registrare il proprio LISA
if (isset($_POST['seriale']))
  {
    $seriale = $_POST['seriale'];
    // controllo se il LISA già è registrato
    $risultato = $conn->query("SELECT * FROM users WHERE codice_LISA=" . $seriale);
    //TODO controllo se il LISA è disponibile per la registrazione
    // se non è registrato da nessuno, associa il LISA all'utente
    if ($risultato->num_rows == 0)
      {
        $query = $conn->prepare("UPDATE users SET codice_LISA = ? WHERE users.id = ?");
        $query->bind_param("ii", $seriale, $_SESSION['user']);
        $query->execute();
        $query->close();
      }
    else
      echo "Errore: LISA già registrato";
  }
// recupera i dati dell'utente che ha effettuato il login
$res = $conn->query("SELECT * FROM users WHERE id=" . $_SESSION['user']);
$userRow = mysqli_fetch_array($res, MYSQLI_ASSOC);
// controllo se l'utente ha registrato il proprio dispositivo LISA
if ($userRow['codice_LISA'] == NULL)
  $stato_LISA = 'non registrato';
else
  $stato_LISA = 'registrato';
?>

<!DOCTYPE html>
<html>
  <head>
    <meta http-equiv="X-UA-Compatible" content="IE=edge">
    <meta charset="utf-8">
    <title></title>
    <meta name="description" content="">
    <meta name="author" content="">
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <link rel="stylesheet" href="">
    <!--[if lt IE 9]>
    <script src="//cdnjs.cloudflare.com/ajax/libs/html5shiv/3.7.2/html5shiv.min.js"></script>
    <script src="//cdnjs.cloudflare.com/ajax/libs/respond.js/1.4.2/respond.min.js"></script>
    <![endif]-->
    <link rel="shortcut icon" href="assets/img/favicon.ico">
  </head>
  <body>
    <p>Username: <?php echo $userRow['username']; ?> </p>
    <p>Email: <?php echo $userRow['email']; ?> </p>
    <p>Stato LISA: <?php echo $stato_LISA; ?></p>
    <?php
    if ($userRow['codice_LISA'] == NULL)
      echo(
      '<p> Inserisci il codice seriale del tuo LISA per registrarlo </p>
      <form method="POST" action="userinfo.php">
        <input type="text" name="seriale">
        <input type="submit">
      </form>'
      );
    ?>
  </body>
</html>