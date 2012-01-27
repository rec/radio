<div id="playerTop">
  <div id="player2">
    <table id="TitleTable">
      <tr>
        <td style="float:left">
          <img src="images/favicon.ico" width="40" height="40"/>
        </td>
        <td align="center">
          <div id="MainTitle">swirly radio international</div>
        </td>
        <td align="right" id="Listeners">
          <div id="ListenersDiv">
            <b id="CurrListeners"/>
          </div>
        </td>
      </tr>
    </table>
    <input id="SongTitleDiv">
    <ul id="SongHistory" />
  </div>
</div>

<table width="100%">
  <tr>
    <td>
      <div id="player_preview" class="box" align="left"></div>
    </td>
    <td>
      <div id="tunein" class="box" align="right">
        <div id="tunelinks" padding="20px">
          <a href="playlist/playlist.pls"><img src="images/itunes.png" /></a>
          <a href="playlist/playlist.pls"><img src="images/winamp.png" /></a>
          <a href="playlist/playlist.asx"><img src="images/wmp.png" /></a>
        </div>
      </div>
    </td>
  </tr>
</table>

<script type="text/javascript" src="js/swfobject.js"></script>

<script type="text/javascript">
  var so = new SWFObject('swf/player.swf', 'mpl', 400, 20, '9');

  so.addParam('allowscriptaccess','always');
  so.addParam('allowfullscreen','true');
  so.addVariable('type', 'sound');
  so.addVariable('file', 'http://radio.swirly.com:8000/radio');
  so.addVariable('autoplay', 'true');
  so.write('player_preview');
</script>
