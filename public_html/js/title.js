var x = 0;
var scrollInterval = 150;
var OFF_THE_AIR = '               ...off the air...            ';
var MISSING_TITLE = '                                                   ';
var songTitle = MISSING_TITLE;
var startPosition = 0;
var scrollingRegion = 300;

function songTitleScroller() {
  var offTheAir = !songTitle;
  var mainMessage = offTheAir ? OFF_THE_AIR : (songTitle + "...         ");
  var tempLoc = Math.max((scrollingRegion * 3 / mainMessage.length) + 1, 1);
  var m = mainMessage;

  for (var counter = 0; counter <= tempLoc; counter++)
    mainMessage += m;

  var s = mainMessage.substring(startPosition,
				startPosition + scrollingRegion);
  document.getElementById('SongTitleDiv').value = s;

  startPosition++;
  if (startPosition > scrollingRegion)
    startPosition = 0;

  setTimeout("songTitleScroller()", (offTheAir ? 4 : 1) *scrollInterval);
}

function start() {
  new Repeater('title', 1000, function(response) {
      if (response.title != songTitle)
        songTitle = response.title;
  });
  new Repeater('listenerCount', 1000, function(response) {
      var listeners = response.listeners || 0;
      document.getElementById('CurrListeners').innerHTML = listeners +
        (' listener' + ((listeners == 1) ? '' : 's') + ' online.');
  });

  songTitleScroller();
};

jQuery(document).ready(start);
