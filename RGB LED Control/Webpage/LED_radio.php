




<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>LED Radio Control</title>
    <style>
    </style>
</head>
<body>

<a href = "index.php">Back</a>

<h1>LED Control</h1>
<form action="LED_radio.php" method="post">
    <h2>LED Status</h2>
    <label>
        <input type="radio" name="LED" value="On" checked> On
    </label>
    <label>
        <input type="radio" name="LED" value="Off"> Off
    </label>
    <br>
    <input type="submit" name="submitStatus" value="Submit Status">
</form>
<script>
    function updateValue(color) {
        document.getElementById(color + 'Value').innerText = document.getElementById(color).value;
    }
</script>

<?php
function saveColorData($data) {
    $json = json_encode($data, JSON_PRETTY_PRINT);
    file_put_contents('results.txt', $json);
}

if ($_SERVER["REQUEST_METHOD"] == "POST") {
    $filePath = 'results.txt';
    
    // Initialize data array
    $colorData = file_exists($filePath) ? json_decode(file_get_contents($filePath), true) : ['Red' => 0, 'Green' => 0, 'Blue' => 0, 'LED' => false];

    if (isset($_POST['submitStatus'])) {
        // Update status
        $colorData['LED'] = ($_POST['LED'] === 'On');
        saveColorData($colorData);
    }
    
    echo "<pre>" . json_encode($colorData, JSON_PRETTY_PRINT) . "</pre>";
}
?>

</body>
</html>
