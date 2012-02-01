// Call the same function repeatedly using a timeout.

var requestIndex = 0;

function requestUrl(url, callback, cached) {
  url = 'generated/' + url + '.json';
  url += (cached ? '' : ('?no-cache=' + (++requestIndex)));
  new Ajax.Request(url,
       {method: 'get',
        parameters: '',
        onComplete: callback});
};


function Repeater(url, updatePeriod, callback, cached) {
  var localCallback;
  this.updatePeriod = updatePeriod || 1000;

  localCallback = function(request) {
    try {
      callback(request.responseText.evalJSON());
    } catch (e) {}
    setTimeout(doRequest, updatePeriod);
  };

  requestUrl(url, localCallback);
};
