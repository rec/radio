var googleMap;
var listenerFetchInterval = 10000;


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

function bounceBroadcaster() {
  googleMap.setBroadcastMarkerAnimation(true);
  setTimeout('unbounceBroadcaster()', 3500);
};


function unbounceBroadcaster() {  
  googleMap.setBroadcastMarkerAnimation(false);
  setTimeout('bounceBroadcaster()', 60000);
};

function startMaps() {
  googleMap = new GoogleMap();
  requestListeners();
  setTimeout("bounceBroadcaster()", 1000);
};

jQuery(document).ready(startMaps);
