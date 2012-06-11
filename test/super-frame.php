<?php

require_once "php/html.php";
require_once "php/messages.php";
loadTranslations("messages/%s.php");
require_once "php/head-frame.php";
 
?>
<body id="SuperFrameBody">
  <div id="outer" background="#000000">
    <div id="inner">
      <iframe id="MainIframe" src="index-frame.php"/>
    </div>
  </div>
</html>
