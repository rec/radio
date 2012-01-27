var googleMap;

function GoogleMap() {
  var swirlyRadio = new google.maps.LatLng(40.719569, -73.961105);
  var config = { center: swirlyRadio,
                 zoom: 1,
                 mapTypeId: google.maps.MapTypeId.ROADMAP };
  var canvas = document.getElementById("map_canvas");
  var map = new google.maps.Map(canvas, config);

  var broadcastMarker = null;

  this.addBroadcastMarker = function(marker) {
    if (broadcastMarker)
      broadcastMarker.setMarker(null);
    var position = new google.maps.LatLng(parseFloat(marker.latitude), parseFloat(marker.longitude));
    broadcastMarker = new google.maps.Marker({
        position: position,
        animation: google.maps.Animation.DROP,
        map: map,
        icon: 'images/ensoo-tiny.gif',
        shadow: 'images/ensoo-shadow-tiny.gif',
        title: marker.title});
  };
};

jQuery(document).ready(function() { googleMap = new GoogleMap(); } );
