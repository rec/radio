var googleMap;
var listenerFetchInterval = 10000;

function GoogleMap() {
  var swirlyRadio = new google.maps.LatLng(40.719569, -73.961105);
  var config = { center: swirlyRadio,
                 zoom: 1,
                 mapTypeId: google.maps.MapTypeId.ROADMAP };
  var canvas = document.getElementById("map_canvas");
  var map = new google.maps.Map(canvas, config);

  var broadcastMarker = null;

  this.removeBroadcastMarker = function() {
    if (broadcastMarker) {
      broadcastMarker.setMap(null);
      broadcastMarker = null;
    }
  };

  this.addBroadcastMarker = function(marker) {
    if (!broadcastMarker) {
      var position = new google.maps.LatLng(marker.latitude, marker.longitude);
      broadcastMarker = new google.maps.Marker({
          position: position,
          animation: google.maps.Animation.DROP,
          map: map,
          icon: 'images/ensoo-tiny.gif',
          shadow: 'images/ensoo-shadow-tiny.gif',
          title: marker.title});
    }
  };

  this.setBroadcastMarkerAnimation = function(run) {
    if (broadcastMarker)
      broadcastMarker.setAnimation(run ? google.maps.Animation.BOUNCE : null);
  };

  var markers = {};

  this.addListenerMarkers = function(listeners) {
    var hasMarkers = false;
    var newMarkers = {};
    for (var i = 0; i < listeners.length; ++i)
      newMarkers[listeners[i].index] = listeners[i];

    var newIndexes = {}, removedIndexes = {};
    var j = 0, k = 0;
    for (var i in markers) {
      if (!newMarkers[i])
        removedIndexes[i] = markers[i];
        hasMarkers = true;
	j++;
    }
    for (var i in newMarkers) {
      if (!markers[i]) {
        newIndexes[i] = newMarkers[i];
        hasMarkers = true;
	k++;
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

function listenersArrived(request) {
  try {
    listeners = request.responseText.evalJSON();
    // alert('listeners! ' + listeners.listeners.length);
    setListenerCount(listeners.listeners.length);
    if (listeners.broadcaster) {
      googleMap.addBroadcastMarker(listeners.broadcaster);
      googleMap.addListenerMarkers(listeners.listeners);
    } else {
      googleMap.removeBroadcastMarker();
      googleMap.addListenerMarkers([]);
    }
  } catch(e) {}
  setTimeout("requestListeners()", listenerFetchInterval);
};

function requestListeners() {
  var url = 'generated/listeners.json?no-cache=' + (++requestIndex);
  var myAjax = new Ajax.Request(url,
                                {method: 'get',
                                 parameters: "",
                                 onComplete: listenersArrived});
};

function startMaps() {
  googleMap = new GoogleMap();
  requestListeners();
};

jQuery(document).ready(startMaps);
