// Call the same function repeatedly using a timeout. 

function Repeater(url, callback, updatePeriod, cached) {
  var index = 0, localCcallback;
  this.updatePeriod = updatePeriod || 1000;

  function doRequest() {
    var newUrl = url + (cached ? '' : ('?no-cache=' + (++index)));
    new Ajax.Request(newUrl,
		     {method: 'get',
		      parameters: '',
		      onComplete: localCallback});		     
  };

  localCallback = function(request) {
    try {
      callback(request.responseText.evalJSON());
    } catch (e) {}
    setTimeout(doRequest, updatePeriod);
  };

  doRequest();
};
