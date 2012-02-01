var x = 0;
var scrollInterval = 150;
var OFF_THE_AIR = '               ...off the air...            ';
var MISSING_TITLE = '                                                   ';
var songTitle = MISSING_TITLE;
var startPosition = 0;
var scrollingRegion = 300;
var songTitleDiv;
var stationOnline;
var innDiv;
var offTheAir = true;
var titleFetchInterval = 3000;

var requestIndex = 0;

function requestAjax(url, callback) {
   new Ajax.Request(url + '?bogus=' + (++requestIndex),
		    {method: 'get',
		     parameters: "",
		     onComplete: callback});
};

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

function setListenerCount(listeners) {
  document.getElementById('CurrListeners').innerHTML = listeners +
    (' listener' + ((listeners == 1) ? '' : 's') + ' online.');
};

function statusArrived(request) {
  try {
    data = request.responseText.evalJSON();
    // songTitle = data.title;
    offTheAir = !!data.error;
    if (offTheAir) 
      songTitle = OFF_THE_AIR;
    else if (!songTitle)
      songTitle = MISSING_TITLE;
    document.getElementById('SongHistory').innerHTML =
      makeScroller(data.titleList || []);
  } catch(e) {}
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

  setTimeout("songTitleScroller()", (offTheAir ? 4 : 1) *scrollInterval);
}

function requestStatus() {
  requestAjax('generated/status.json', statusArrived);
};

function requestTitle() {
  requestAjax('generated/title.json', titleArrived);
};

function titleArrived(request) {
  try {
    var title = request.responseText.evalJSON().title;
    if (title != songTitle) {
      songTitle = title;
      requestStatus();
    }
  } catch (e) {}
  setTimeout("requestTitle()", titleFetchInterval);
};

jQuery(document).ready(
  function() {
    songTitleScroller();
    requestTitle();
    requestStatus();
  }
);
