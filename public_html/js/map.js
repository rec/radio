var googleMap;

function GoogleMap() {
  var swirlyRadio = new google.maps.LatLng(40.719569, -73.961105);
  var config = { center: swirlyRadio,
                 zoom: 1,
                 mapTypeId: google.maps.MapTypeId.ROADMAP };
  var canvas = document.getElementById("map_canvas");
  var map = new google.maps.Map(canvas, config);

  var broadcastMarker = null;

  this.addBroadcastMarker = function(marker, bounce) {
    if (broadcastMarker)
      broadcastMarker.setMap(null);
    var position = new google.maps.LatLng(marker.latitude, marker.longitude);
    broadcastMarker = new google.maps.Marker({
        position: position,
        animation: (bounce ? google.maps.Animation.BOUNCE :
                    google.maps.Animation.DROP),
        map: map,
        icon: 'images/ensoo-tiny.gif',
        shadow: 'images/ensoo-shadow-tiny.gif',
        title: marker.title});
  };

  var markers = {};

  this.addListenerMarkers = function(listeners) {
    var hasMarkers = false;
    var newMarkers = {};
    for (var i = 0; i < listeners.length; ++i)
      newMarkers[listeners[i].index] = listeners[i];

    var newIndexes = {}, removedIndexes = {};
    for (var i in markers) {
      if (!newMarkers[i])
        removedIndexes[i] = markers[i];
        hasMarkers = true;
    }
    for (var i in newMarkers) {
      if (!markers[i]) {
        newIndexes[i] = newMarkers[i];
        hasMarkers = true;
      }
    }

    if (!hasMarkers)
      return;

    for (var i in newIndexes) {
      var item = newIndexes[i];
      var pos = new google.maps.LatLng(parseFloat(item.geocode.latitude),
                                       parseFloat(item.geocode.longitude));
      markers[i] = new google.maps.Marker({
          position: pos,
          map: map,
          title: item.title});
    }

    for (var i in removedIndexes) {
      markers[i].setMap(null);
      delete markers[i];
    }
  };
};

jQuery(document).ready(function() { googleMap = new GoogleMap(); } );
