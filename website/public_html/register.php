<?php
ob_start();
session_start();

if (isset($_SESSION['user']) != "") {
    header("Location: index.php");
}
include_once 'dbconnect.php';

if (isset($_POST['signup'])) 
  {

      $uname = trim($_POST['uname']); // ottieni dati ricevuti in POST rimuovendo gli spazi agli estremi
      $email = trim($_POST['email']);
      $upass = trim($_POST['pass']);
      $serial = trim($_POST['serial']);

      // hash della password con SHA-256
      $password = hash('sha256', $upass);

      // controllo se l'email esiste oppure no
      $stmt = $conn->prepare("SELECT email FROM users WHERE email=?");
      $stmt->bind_param("s", $email);
      $stmt->execute();
      $result = $stmt->get_result();
      $stmt->close();

      $count = $result->num_rows;

      if ($count == 0) 
        { // se l'email non viene trovata, aggiungi utente
          // controllo se il LISA già è registrato
          $risultato = $conn->query("SELECT * FROM users WHERE codice_LISA=" . $serial);
          // se non è registrato da nessuno, associa il LISA all'utente
          if ($risultato->num_rows == 0)
            {
              $query = $conn->prepare("INSERT INTO users(username, email, password, codice_LISA) VALUES(?, ?, ?, ?)");
              $query->bind_param("sssi", $uname, $email, $password, $serial);
              $res = $query->execute();
              $query->close();
              $user_id = mysqli_insert_id($conn);
              if ($user_id > 0) 
                {
                  $_SESSION['user'] = $user_id; // setta la variabile di sessione e redirect alla dashboard
                  if (isset($_SESSION['user'])) 
                    {
                      print_r($_SESSION);
                      header("Location: index.php");
                      exit;
                    }
                } 
              else 
                {
                  $errTyp = "danger";
                  $errMSG = "Qualcosa è andato storto, prova di nuovo";
                }
            }
          else 
            {
              $errTyp = "warning";
              $errMSG = "LISA già registrato";
            }
        } 
      else 
        {
          $errTyp = "warning";
          $errMSG = "Email già in uso";
        }
  }
?>
<!DOCTYPE html>
<head>
    <meta http-equiv="Content-Type" content="text/html; charset=utf-8"/>
    <title>Registrazione</title>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <link rel="stylesheet" href="assets/css/bootstrap.min.css" type="text/css"/>
    <link rel="stylesheet" href="assets/css/style.css" type="text/css"/>
    <link rel="shortcut icon" href="assets/img/favicon.ico">
</head>
<body>

<div class="container">

    <div id="login-form">
        <form method="post" autocomplete="off">

            <div class="col-md-12">

                <div class="form-group">
                    <h2 class="">Registrati per utilizzare il tuo LISA</h2>
                </div>

                <div class="form-group">
                    <hr/>
                </div>

                <?php
                if (isset($errMSG)) {

                    ?>
                    <div class="form-group">
                        <div class="alert alert-<?php echo ($errTyp == "success") ? "success" : $errTyp; ?>">
                            <span class="glyphicon glyphicon-info-sign"></span> <?php echo $errMSG; ?>
                        </div>
                    </div>
                    <?php
                }
                ?>

                <div class="form-group">
                    <div class="input-group">
                        <span class="input-group-addon"><span class="glyphicon glyphicon-user"></span></span>
                        <input type="text" name="uname" class="form-control" placeholder="Inserisci username" required/>
                    </div>
                </div>

                <div class="form-group">
                    <div class="input-group">
                        <span class="input-group-addon"><span class="glyphicon glyphicon-envelope"></span></span>
                        <input type="email" name="email" class="form-control" placeholder="Inserisci Email" required/>
                    </div>
                </div>

                <div class="form-group">
                    <div class="input-group">
                        <span class="input-group-addon"><span class="glyphicon glyphicon-lock"></span></span>
                        <input type="password" name="pass" class="form-control" placeholder="Inserisci Password"
                               required/>
                    </div>
                </div>

                <div class="form-group">
                  <div class="input-group">
                    <input type="text" name="serial" class="form-control" placeholder="Codice LISA" required/>
                  </div>
                </div>

                <div class="checkbox">
                    <label><input type="checkbox" id="TOS" value="This"><a href="#">Aderisco ai termini del servizio</a></label>
                </div>

                <div class="form-group">
                    <button type="submit" class="btn    btn-block btn-primary" name="signup" id="reg">Registrati</button>
                </div>

                <div class="form-group">
                    <hr/>
                </div>

                <div class="form-group">
                    <a href="login.php" type="button" class="btn btn-block btn-success" name="btn-login">Login</a>
                </div>

            </div>

        </form>
    </div>

</div>
<script type="text/javascript" src="https://ajax.googleapis.com/ajax/libs/jquery/2.2.4/jquery.min.js"></script>
<script type="text/javascript" src="assets/js/bootstrap.min.js"></script>
<script type="text/javascript" src="assets/js/tos.js"></script>

</body>
</html>
