<?php
ob_start();
session_start();
require_once 'dbconnect.php';

if (!isset($_SESSION['user'])) {
    header("Location: login.php");
    exit;
}
// seleziona le informazioni dell'utente loggato
$res = $conn->query("SELECT * FROM users WHERE id=" . $_SESSION['user']);
$userRow = mysqli_fetch_array($res, MYSQLI_ASSOC);
// seleziona le posizioni del dispositivo associato all'utente
$risultato = $conn->query("SELECT posizioni.* 
                            FROM users
                            JOIN LISAs ON LISAs.codice_seriale=users.codice_LISA
                            JOIN posizioni ON LISAs.codice_seriale=posizioni.codice_LISA
                            WHERE users.id=".$userRow['id']);
$posizioni = mysqli_fetch_all($risultato, MYSQLI_ASSOC);

?>
<!DOCTYPE html>
<head>
    <meta http-equiv="Content-Type" content="text/html; charset=utf-8"/>
    <title>Ciao, <?php echo $userRow['username']; ?></title>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <link rel="stylesheet" href="assets/css/bootstrap.min.css" type="text/css"/>
    <link rel="stylesheet" href="assets/css/index.css" type="text/css"/>
    <link rel="shortcut icon" href="assets/img/favicon.ico">
</head>
<body>

<!-- Navigation Bar-->
<nav class="navbar navbar-default navbar-fixed-top">
    <div class="container">
        <div class="navbar-header">
            <button type="button" class="navbar-toggle collapsed" data-toggle="collapse" data-target="#navbar"
                    aria-expanded="false" aria-controls="navbar">
                <span class="sr-only">Toggle navigation</span>
                <span class="icon-bar"></span>
                <span class="icon-bar"></span>
                <span class="icon-bar"></span>
            </button>
            <a class="navbar-brand" href="info.html">Project LISA</a>
        </div>
        <div id="navbar" class="navbar-collapse collapse">
            <ul class="nav navbar-nav">
                <li><a href=".">Dashboard</a></li>
                <li><a href="userinfo.php">Profilo</a></li>
            </ul>
            <ul class="nav navbar-nav navbar-right">

                <li class="dropdown">
                    <a href="#" class="dropdown-toggle" data-toggle="dropdown" role="button" aria-haspopup="true"
                       aria-expanded="false">
                        <span
                            class="glyphicon glyphicon-user"></span>&nbsp;
                           <?php echo $userRow['username']; ?>
                        &nbsp;<span class="caret"></span></a>
                    <ul class="dropdown-menu">
                        <li><a href="logout.php?logout"><span class="glyphicon glyphicon-log-out"></span>&nbsp;Logout</a>
                        </li>
                    </ul>
                </li>
            </ul>
        </div>
    </div>
</nav>

<div class="container">
    <div class="row">
        <div class="col-lg-12">
            <h1>Ciao, <?php echo $userRow['username']; ?></h1>
            <p>Qui sotto puoi visualizzare le ultime posizioni del tuo dispositivo LISA</p>
            <div id="map" style="height: 512px"></div>
        </div>
    </div>
</div>

<script>
  function initMap() {
  var map = new google.maps.Map(document.getElementById('map'), {
    zoom: 3,
    // centra la mappa sulla prima coppia di coordinate
    center: <?php printf("{lat: %f, lng: %f},", $posizioni[0]['latitudine'], $posizioni[0]['longitudine'])  ?>
    mapTypeId: 'terrain'
  });

  var coordinatePercorso = [
    // recupera coordinate e orario e li inserisce nell'array
    <?php
        foreach($posizioni as $riga)
          {
            printf("{lat: %f, lng: %f, time: 'Posizione registrata alla seguente data/ora:<br>%s'},\n", $riga['latitudine'], $riga['longitudine'], $riga['data_ora']);
          }
      ?>
  ];

  var infowindow = new google.maps.InfoWindow();
  var i;

  // crea i marker e le finestre informative per ogni coppia di coordinate nell'array
  for (i = 0; i < coordinatePercorso.length; i++) {
    var marker = new google.maps.Marker({
      position: coordinatePercorso[i],
      map: map,
      title: 'Hello World!'
    });

    google.maps.event.addListener(marker, 'click', (function(marker, i) {
      return function() {
        infowindow.setContent(coordinatePercorso[i].time);
        infowindow.open(map, marker);
      }
    })(marker, i));
  }

  // crea le linee d'aria tra una coppia di coordinate e l'altra
  var percorso = new google.maps.Polyline({
    path: coordinatePercorso,
    geodesic: true,
    strokeColor: '#FF0000',
    strokeOpacity: 1.0,
    strokeWeight: 2
  });

  percorso.setMap(map);
}
</script>
<script async defer src="https://maps.googleapis.com/maps/api/js?key=YOUR_API_KEY&callback=initMap"></script>
<script src="https://ajax.googleapis.com/ajax/libs/jquery/2.2.4/jquery.min.js"></script>
<script src="assets/js/bootstrap.min.js"></script>

</body>
</html>
