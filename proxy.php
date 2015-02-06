<?php
$URL = $_GET["url"];
$file = fopen("$URL", "r");

while(!feof($file)) {
	$line = fgets($file);
	echo $line;
}

?>
