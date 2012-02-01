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

function start() {
  new Repeater('status', 3000, function(response) {
      document.getElementById('SongHistory').innerHTML =
        makeScroller(response.titleList || []);
  });
};

jQuery(document).ready(start);
