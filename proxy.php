<?php

	$cSession = curl_init(); 

	curl_setopt($cSession,CURLOPT_URL,"http://www.google.com/search?q=youtube");
	curl_setopt($cSession,CURLOPT_RETURNTRANSFER,true);
	curl_setopt($cSession,CURLOPT_HEADER, false); 

	$result=curl_exec($cSession);

	curl_close($cSession);

	echo $result;
?>
