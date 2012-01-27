var x = 0;
var scrollInterval = 150;
var songTitle = '(no song)';
var startPosition = 0;
var scrollingRegion = 210;
var songTitleDiv;
var stationOnline;
var innDiv;
var fetchInterval = 1000;

var parts = [
    '<li class="lines"' +
    'style="' +
    'font-size:13px; ' +
    'line-height: 20px; ' +
    'height: 20px; ' +
    'overflow: hidden; ' +
    'border-bottom: 1px dotted #CCC; ' +
    'color: #333; ',

    '"> <span style="color:#AF7B00;">',

    '</span> ',

    '</li>'];

var alternates = ['background: #F0F0F0',
                  'background: #CCC'];

function makeScrollerLine(titleRecord) {
  return [parts[0],
          alternates[titleRecord.index % alternates.length],
          parts[1],
          titleRecord.time,
          parts[2],
          titleRecord.title,
          parts[3]].join('');
};

function makeScroller(titleList) {
  var scroller = [];
  for (var i = 0; i < titleList.length; ++i)
    scroller[i] = makeScrollerLine(titleList[i]);
  return scroller.join('\n');
};

function requestStatus() {
  var url = "generated/status.json";
  var myAjax = new Ajax.Request(url,
                                {method: 'get',
                                 parameters: "",
                                 onComplete: statusArrived});
};

function setListenerCount(listeners) {
  document.getElementById('CurrListeners').innerHTML = listeners +
    (' listener' + ((listeners == 1) ? '' : 's') + ' online.');
};

function statusArrived(request) {
  try {
    data = request.responseText.evalJSON();
    songTitle = data.title || '(no song)';
    setListenerCount(data.listeners || 0);
    document.getElementById('SongHistory').innerHTML =
      makeScroller(data.titleList || []);
  } catch(e) {}
  setTimeout("requestStatus()", fetchInterval);
};

function requestListeners() {
  var url = "generated/listeners.json";
  var myAjax = new Ajax.Request(url,
                                {method: 'get',
                                 parameters: "",
                                 onComplete: listenersArrived});
};

function listenersArrived(request) {
  try {
    listeners = request.responseText.evalJSON();
    setListenerCount(listeners.listeners.length);
    googleMap.addBroadcastMarker(listeners.broadcaster, true);
    googleMap.addListenerMarkers(listeners.listeners);
    googleMap.addBroadcastMarker(listeners.broadcaster, false);
  } catch(e) {}
  setTimeout("requestStatus()", fetchInterval);
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
    requestStatus();
    requestListeners();
  }
);
