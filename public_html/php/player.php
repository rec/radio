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
    <ul id="SongHistory"/>
  </div>
</div>

<div id="player_preview" class="box" align="center"></div>

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
