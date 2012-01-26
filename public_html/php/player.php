<div id="playerTop">
  <div id="player2">
    <div id="MainTitle">
      swirly radio international
    </div>
    <div id="StationOnline">
      <input id="SongTitleDiv">
      <div id="ListenersDiv"><b id="CurrListeners"></b></div>
    </div>

    <div id="LastPlayedSongsDiv">
      <span id="LastPlayedSongs">Last played songs</span>
    </div>

    <ul id="SongHistory"/>
  </div>
</div>

<script type="text/javascript" src="js/swfobject.js"></script>

<div id="player_preview" class="box"></div>

<script type="text/javascript">
  var so = new SWFObject('swf/player.swf', 'mpl', 500, 20, '9');

  so.addParam('allowscriptaccess','always');
  so.addParam('allowfullscreen','true');
  so.addVariable('type', 'sound');
  so.addVariable('file', 'http://radio.swirly.com:8000/radio');
  so.addVariable('autoplay', 'true');
  so.write('player_preview')

</script>
