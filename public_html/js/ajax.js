var requestIndex = 0;

function requestAjax(url, callback) {
  new Ajax.Request(url + '?bogus=' + (++requestIndex), 
		   {method: 'get', parameters: '', onComplete: callback});
};

