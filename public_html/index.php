<?php

require_once "php/html.php";
require_once "php/messages.php";
loadTranslations("messages/%s.php");
require_once "php/head.php";

?>

<body>
  <div id="outer">
    <div id="inner">
      <img src="images/favicon.ico"/>
      <?php require "php/player.php"; ?>
      <?php require "php/twitter.php"; ?>
<!--      <?php require "php/tune-in.php"; ?> -->
      <?php require "php/footer.php"; ?>
    </div>
  </div>
</body>

</html>

