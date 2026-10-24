// Phone location is requested once, only after the player saves a match.
Pebble.addEventListener('appmessage', function (event) {
  var payload = event.payload || {};
  if (payload.LOC_REQUEST !== 1) return;

  var slot = payload.LOC_SLOT;
  function report(status, latitude, longitude, accuracy) {
    Pebble.sendAppMessage({
      LOC_SLOT: slot,
      LOC_STATUS: status,
      LOC_LAT_E6: latitude,
      LOC_LON_E6: longitude,
      LOC_ACCURACY_M: accuracy
    });
  }

  navigator.geolocation.getCurrentPosition(function (position) {
    var latitude = Math.round(position.coords.latitude * 1000000);
    var longitude = Math.round(position.coords.longitude * 1000000);
    var accuracy = Math.min(65535, Math.max(0, Math.round(position.coords.accuracy)));
    report(1, latitude, longitude, accuracy);
  }, function () {
    report(0, 0, 0, 0);
  }, {
    enableHighAccuracy: true,
    maximumAge: 30000,
    timeout: 12000
  });
});
