
function createSWFPlayer(container) {
  var so = new SWFObject('swf/player.swf', 'mpl', 400, 24, '9');

  so.addParam('allowscriptaccess','always');
  so.addParam('allowfullscreen','true');
  so.addVariable('type', 'sound');
  so.addVariable('file', 'http://radio.swirly.com:8000/radio');
  so.addVariable('autoplay', 'true');
  so.write(container || 'player_preview');
};
