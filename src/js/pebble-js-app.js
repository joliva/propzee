var locationOptions = {
	enableHighAccuracy: true, 
	maximumAge: 10000, 
	timeout: 10000
};

function locationSuccess(pos) {
	var lat = pos.coords.latitude;
	var lon = pos.coords.longitude;
	
	//lat = 40.4203143;
	//lon = -74.3901083;

	console.log('lat = ' + lat + ' lon = ' + lon);

	var req = new XMLHttpRequest();

	req.open('GET', 'http://locator.olivalabs.com/api/infoLatLon?lat=' + lat + '&lon=' + lon, true);
	req.onload = function(e) {
		if (req.readyState == 4 && req.status == 200) {
			if(req.status == 200) {
				var response = JSON.parse(req.responseText);
				console.log(JSON.stringify(response, null, 2));

				if (response.err === null) {
					Pebble.openURL(response.zillow.link);
				}
			} else { 
				console.log('Error');
			}
		}
	};

	req.send(null);
}

function locationError(err) {
	console.log('location error (' + err.code + '): ' + err.message);
}

Pebble.addEventListener("ready",
		function(e) {
			//console.log("Hello world! - Sent from your javascript application.");
		}
);

Pebble.addEventListener('appmessage',
	function(e) {
		//console.log('Received message: ' + JSON.stringify(e.payload));

		// Request current position
		navigator.geolocation.getCurrentPosition(locationSuccess, locationError, locationOptions);
	}
);
