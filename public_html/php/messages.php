<?php

$LANGUAGES = array("de", "en", "es", "fr", "id");

// MESSAGES is an array mapping languages to translation tables.
// A translation table is an array mapping message names to translations for
// that message.

$MESSAGES = array();

foreach ($LANGUAGES as $l)
  $MESSAGES[$l] = array();

// Look through the browser string for a language we understand and return it,
// else return "en".
function getBrowserLanguage() {
  if (isset($_SERVER["HTTP_ACCEPT_LANGUAGE"])) {
    $acceptLanguage = $_SERVER["HTTP_ACCEPT_LANGUAGE"];
    global $LANGUAGES;
    $parts = split(";", $acceptLanguage);
    foreach (split(",", $parts[0]) as $langRegion) {
      $lang = split("-", $langRegion);
      if (in_array($lang[0], $LANGUAGES))
	return $lang[0];
    }
  }

  return "en";
}

$LANGUAGE = getBrowserLanguage();

function loadTranslations($filepattern) {
  global $LANGUAGE;
  require_once sprintf($filepattern, "en");
  include_once sprintf($filepattern, $LANGUAGE);
}

// Echo a translated message or log an error.
function MSG($message) {
  global $MESSAGES;
  global $LANGUAGE;
  global $HTTP_ACCEPT_LANGUAGE;
  $translation = $MESSAGES[$LANGUAGE][$message];
  if (!isset($translation))
    $translation = $MESSAGES["en"][$message];

  if (!isset($translation)) {
    $translation = "?" . $message . "?";
    $error = sprintf("Couldn't find message '%s' in '%s'",
                      $message, $HTTP_ACCEPT_LANGUAGE, $LANGUAGE);
    error_log($error);
  }

  echo $translation;
}

?>
