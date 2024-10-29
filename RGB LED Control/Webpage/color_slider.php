<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>RGB Color Slider</title>
    <style>
        .slider {
            width: 300px;
        }
    </style>
</head>
<body>

<a href = "index.php">Back</a>

<h1>RGB Color Control</h1>
<form action="color_slider.php" method="post">
    <label for="r">Red: <span id="rValue">0</span></label>
    <input type="range" id="r" name="r" min="0" max="255" value="0" class="slider" oninput="updateValue('r')"><br>

    <label for="g">Green: <span id="gValue">0</span></label>
    <input type="range" id="g" name="g" min="0" max="255" value="0" class="slider" oninput="updateValue('g')"><br>

    <label for="b">Blue: <span id="bValue">0</span></label>
    <input type="range" id="b" name="b" min="0" max="255" value="0" class="slider" oninput="updateValue('b')"><br>

    <input type="submit" name="submitColor" value="Submit RGB Values">
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

    if (isset($_POST['submitColor'])) {
        // Update RGB values
        $colorData['Red'] = intval($_POST['r']);
        $colorData['Green'] = intval($_POST['g']);
        $colorData['Blue'] = intval($_POST['b']);
        saveColorData($colorData);
    }
    
    echo "<pre>" . json_encode($colorData, JSON_PRETTY_PRINT) . "</pre>";
}
?>

</body>
</html>
