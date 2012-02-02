<?php

require_once "php/html.php";
require_once "php/messages.php";
loadTranslations("messages/%s.php");

?>

<html>
<head>
  <link href="new-style.css" rel="stylesheet" type="text/css"/>
  <script type="text/javascript" src="js/swfplayer.js"></script>
  <script type="text/javascript" src="js/swfobject.js"></script>
</head>

<body>
  <div id="footer">
    <table width="100%" height="100%">
      <tr style="vertical-align: top">
	<td style="float:left; vertical-align: top">
	  <?php MSG("broadcasting since") ?>
	</td>

	<td style="float:right" align="right" id="Copyright">
	  <?php MSG("copyright") ?>
	  <a href="mailto:tom@swirly.com" style="position:relative; top: -1px;" >Tom Swirly</a>.
	  <?php MSG("all rights reserved") ?>
	</td>
      </tr>
    </table>
  </div>
</body>
