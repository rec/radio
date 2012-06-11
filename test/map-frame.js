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
  setTimeout('unbounceBroadcaster()', Common.Delay.unbounce);
};


function unbounceBroadcaster() {
  googleMap.setBroadcastMarkerAnimation(false);
  setTimeout('bounceBroadcaster()', Common.Delay.bounce);
};

function startMaps() {
  googleMap = new GoogleMap();
  new Repeater('listeners', Common.Delay.startMap, listenersArrived);

  if (Common.Enable.bounce)
    setTimeout(bounceBroadcaster, Common.Delay.initialBounce);
};

jQuery(document).ready(startMaps);
