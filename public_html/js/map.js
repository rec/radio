var googleMap;

function listenersArrived(request) {
  try {
    listeners = request.responseText.evalJSON();
    setListenerCount(listeners.listeners.length);
    if (listeners.broadcaster) {
      googleMap.addBroadcastMarker(listeners.broadcaster);
      googleMap.addListenerMarkers(listeners.listeners);
    } else {
      googleMap.removeBroadcastMarker();
      googleMap.addListenerMarkers([]);
    }
  } catch(e) {}

  setTimeout("requestListeners()", Common.Delay.listenerFetch);
};

function requestListeners() {
  var url = 'generated/listeners.json?no-cache=' + (++requestIndex);
  var myAjax = new Ajax.Request(url,
                                {method: 'get',
                                 parameters: "",
                                 onComplete: listenersArrived});
};

function startMaps() {
  googleMap = new GoogleMap('map_canvas');
  requestListeners();
};

jQuery(document).ready(startMaps);
