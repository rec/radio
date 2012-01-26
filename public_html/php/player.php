<div id="playerTop">
   <div id="player2">
     <div id="MainTitle">
       swirly radio international
     </div>
   <div id="StationOnline">
     <input id="SongTitleDiv">

     <div id="ListenersDiv">
       <b id="CurrListeners"></b></div>
   </div>

   <div id="LastPlayedSongsDiv" style="color:#AF7B00; font-size:13px; height:20px; margin-top:20px;
	       padding-bottom:5px;  font-weight:bold; border-bottom:1px dotted #CCC ">
     <span style="float:left; margin-right:10px; width:130px;">Last played songs</span>
       </div>
   <ul style="position:absolute; left:20px;padding:0px; color:#666;
              line-height:14px; font-size:12px; list-style:none;
              height:280px; margin-top:10px; overflow:auto; " id="SongHistory">
   </ul>

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

<!--
<object width="271" height="75" bgcolor="#E6E6E6">
  <param name="movie" value="player_4/flash.swf" />
  <param name="flashvars"
         value="url=http://radio.swirly.com:8000/radio/live&autoplay=true&lang=en&codec=mp3&volume=75&traking=true&jsevents=false&buffering=5&skin=player_4/xml_file.xml&title=swirly%20radio%20international&welcome=WELCOME%20TO..." />
  <param name="wmode" value="window" />
  <param name="allowscriptaccess" value="always" />
  <param name="scale" value="noscale" />
  <embed src="player_4/flash.swf" flashvars="url=http://radio.swirly.com:8000/radio/live&lang=en&autoplay=true&codec=mp3&volume=75&traking=true&jsevents=false&buffering=5&skin=player_4/xml_file.xml&title=radio%20swirly%20international&welcome=WELCOME%20TO..."
         width="271"
         scale="noscale"
         height="75"
         wmode="window"
         bgcolor="#E6E6E6"
         allowscriptaccess="always"
         type="application/x-shockwave-flash" />
</object>
-->
