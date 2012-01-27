var googleMap;

function GoogleMap() {
  var swirlyRadio = new google.maps.LatLng(40.719569, -73.961105);
  var config = { center: swirlyRadio,
                 zoom: 1,
                 mapTypeId: google.maps.MapTypeId.ROADMAP };
  var canvas = document.getElementById("map_canvas");
  var map = new google.maps.Map(canvas, config);

  var ensoo = new google.maps.MarkerImage(
      'images/ensoo.gif',
      new google.maps.Size(32, 32),
      new google.maps.Point(0,0),
      new google.maps.Point(16, 16));

  var ensoo_shadow = new google.maps.MarkerImage(
      'images/ensoo_shadow.gif',
      new google.maps.Size(32, 32),
      new google.maps.Point(0,0),
      new google.maps.Point(16, 16));

  var marker = new google.maps.Marker({
      position: swirlyRadio,
      animation: google.maps.Animation.DROP,
      map: map,
      icon: 'images/ensoo-tiny.gif',
      shadow: 'images/ensoo-shadow-tiny.gif',
      title:'swirly radio international'});
};

jQuery(document).ready(function() { new GoogleMap(); } );
