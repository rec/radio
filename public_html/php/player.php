<script type="text/javascript" src="js/prototype.js"></script>
<script type="text/javascript" src="js/jquery.js"></script>
<script type="text/javascript">
           jQuery.noConflict();
</script>

<script type="text/javascript" src="js/player.js"></script>
           
<div style="background:#F0F0F0; float:left; clear:left; width:500px;
            height:420px; text-align:left; font-family:Arial;">
   <div style="padding:6px; float:left; clear:left; width:488px">
     <div style="color:#AF7B00; font-weight:bold; font-size:16px; 
                 text-align:left; width:100%; float:left; margin-bottom:0px; " 
          id="MainTitle">
       swirly radio international
     </div>
   <div style="display: block; " id="StationOnline">
     <input style="color:#333; line-height:20px; padding:10px 0; font-size:14px;
		               width:490px; font-weight:bold; border:none; background:none; 
                   float:right; clear:right;" 
            id="SongTitleDiv">
     <div style="color:#666; line-height:14px; font-size:12px;" id="ListenersDiv">
       <b id="CurrListeners"></b></div>
   </div>

   <div style="color: red; font-size: 14px; font-weight: bold; display: none; " id="StationOffline">
   </div>

   <div style="color:#AF7B00; font-size:13px; height:20px; margin-top:20px;
	       padding-bottom:5px;  font-weight:bold; border-bottom:1px dotted #CCC ">
     <span style="float:left; margin-right:10px; width:130px;">Last played songs</span>
       </div>
   <ul style="position:absolute; left:20px;padding:0px; color:#666; line-height:14px; font-size:12px; list-style:none; width:488px; height:280px; margin-top:10px; overflow:auto; " id="SongHistory">  
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
  so.addVariable('file', 'http://radio.swirly.com:8000/swirlymount');
  so.write('player_preview')

</script>

