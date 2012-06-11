<?php

require_once "php/html.php";
require_once "php/messages.php";
loadTranslations("messages/%s.php");
require_once "php/head-frame.php";
 
?>
<body>
  <iframe src="title-frame.php"/>
  <iframe src="songs-frame.php"/>
  <iframe src="player-frame.php"/>
  <iframe src="twitter-frame.php"/>
  <iframe src="map-frame.php"/>
  <iframe src="footer-frame.php"/>
  
</body>
</html>
