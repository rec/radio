var x = 0;
var scrollInterval = 150;
var songTitle = '(no song)';
var startPosition = 0;
var scrollingRegion = 80;
var songTitleDiv;
var stationOnline;
var innDiv;
var fetchInterval = 1000;

function requestSongJson() {  
  var url = "generated/json.html";
  var myAjax = new Ajax.Request( url,  {method: 'get',  parameters: "", 
	onComplete: jsonArrived});
};

function jsonArrived(originalRequest) {
  try {
    data = originalRequest.responseText.evalJSON();
    songTitle = data.title || '(no song)';
    var listeners = data.listeners || 0;
    listeners += (' listener' + ((listeners == 1) ? '' : 's') + ' online');
    document.getElementById('CurrListeners').innerHTML = listeners;
    document.getElementById('SongHistory').innerHTML = data.scroller;
  } catch(e) {}
  setTimeout("requestSongJson()", fetchInterval);
};

function songTitleScroller() {
  var mainMessage = songTitle + "...         ";
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
    
  setTimeout("songTitleScroller()", scrollInterval); 
}

jQuery(document).ready(
  function() {
    songTitleScroller();
    requestSongJson();
  }
);
