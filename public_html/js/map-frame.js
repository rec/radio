var googleMap;

function listenersArrived(listeners) {
  if (listeners.broadcaster) {
    googleMap.addBroadcastMarker(listeners.broadcaster);
    googleMap.addListenerMarkers(listeners.listeners);
  } else {
    googleMap.removeBroadcastMarker();
    googleMap.addListenerMarkers([]);
  }
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
  new Repeater('listeners', 10000, listenersArrived);

  setTimeout(bounceBroadcaster, 1000);
};

jQuery(document).ready(startMaps);
