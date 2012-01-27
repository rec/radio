var googleMap;

function GoogleMap() {
  var swirlyRadio = new google.maps.LatLng(40.719569, -73.961105);
  var config = { center: swirlyRadio,
                 zoom: 1,
                 mapTypeId: google.maps.MapTypeId.ROADMAP };
  var canvas = document.getElementById("map_canvas");
  var map = new google.maps.Map(canvas, config);
  var marker = new google.maps.Marker({
      position: swirlyRadio,
      animation: google.maps.Animation.DROP,
      map: map,
      icon: 'images/favico.gif',
      title:'swirly radio international'});
};

jQuery(document).ready(function() { new GoogleMap(); } );
