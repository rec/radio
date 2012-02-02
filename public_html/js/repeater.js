// Call the same function repeatedly using a timeout.

var requestIndex = 0;

function callSafe(callback, value) {
  try {
    callback(value);
  } catch (e) {}
};

function makeCallSafe(callback) {
  return function(value) {
    return callSafe(callback, value);
  };
};

function requestUrl(url, callback, cached) {
  url = 'generated/' + url + '.json';
  url += (cached ? '' : ('?no-cache=' + (++requestIndex)));

  function localCallback(request) {
    callSafe(callback, request.responseText.evalJSON());
  };

  new Ajax.Request(url,
       {method: 'get',
        parameters: '',
        onComplete: localCallback});
};


function Repeater(url, updatePeriod, callback, cached) {
  var localCallback;

  function doRequest() {
    requestUrl(url, localCallback, cached);
  };

  localCallback = function(request) {
    callSafe(callback, request);
    setTimeout(doRequest, updatePeriod);
  };

  doRequest();
};
