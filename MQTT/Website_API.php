<!DOCTYPE html>
<html lang="en">
<head>
</head>
<body>


<?php
$servername = "localhost";
$username = "XXXXXXXXXX";
$password = "XXXXXXXXXX";
$dbname = "XXXXXXXXXXX";

// Create connection
$conn = new mysqli($servername, $username, $password, $dbname);

// Check connection
if ($conn->connect_error) {
    echo "connection error.";
    die("Connection failed: " . $conn->connect_error);
    
}


$potValue = NULL;


foreach ($_REQUEST as $key => $value)
{
    if($key == "pot")
    {
        $potValue = $value;
        //echo $nodeParam;
        // "<br>";
    }
}


if (isset($potValue)){
    $sql = "INSERT INTO `MQTT_Data`(`sensor_value`) VALUES ('" .$potValue . "')";

    $result = $conn->query($sql);
}

?>

Text
</body>
</html>
