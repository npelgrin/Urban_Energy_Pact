<?php
    date_default_timezone_set('Europe/Paris');
    $dater = date("Y-m-d");
    $heure = date("H:i:s");
    if (!isset($_GET['data']))
    {
     $datas = "rien";
    }
    else
    	$datas = $_GET["data"];

    $servername = '127.0.0.1';
    $dbname = 'gergovie';
    $username = 'gergovie';
    $password = 'gergovie';

    try {
        $conn = new PDO("mysql:host=$servername;dbname=$dbname", $username, $password);
        // set the PDO error mode to exception
        $conn->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);
        $sql = "INSERT INTO data_capteurs (valeur, heure, jour)
        VALUES ('$datas', '$heure', '$dater')";
        // use exec() because no results are returned
        $conn->exec($sql);
        echo "New record created successfully";
      } catch(PDOException $e) {
        echo $sql . "<br>" . $e->getMessage();
      }
      
      $conn = null;
?>

<!doctype html>
<html lang="fr">

<head>
	<meta charset="utf-8">
	<title> Page d'essai reception données </title>
	<link rel="stylesheet" href="styles/style.css">
</head>

<body>
	<div class="page">
		<p>Affichage de la donnée</p>
	
		<?php
			Print("Nous sommes le $dater et il est $heure");
		?>
		<p></p>
			<td> Température : </td>
			<td>
            <?php
            echo "$datas";
            ?>
			</td>

	</div>
</body>

</html>