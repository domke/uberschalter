$(document).ready(function() {
	for(lamp = 1; lamp <= 6; lamp++) {
		// Schalter initalisieren
		$("[name='lamp-" + lamp + "']").bootstrapSwitch();
		
		// Eventhandler
		$("[name='lamp-" + lamp + "']").on('switchChange.bootstrapSwitch', function(event, state) {
			update(event.target.name, state);
		});
	}

	$(":button").on("click", function(event, state) {
		name = event.target.name;
		if(name.substr("-3") == "-on") {
			update(name.substring(0, name.length - 3), true);
		}
		if(name.substr("-4") == "-off") {
			update(name.substring(0, name.length - 4), false);
		}
	});

	query();
});

function update(source, state) {
	element = source.split("-")[1]
	if(state == true) {
		value = "on";
	}
	else {
		value = "off";
	}

	console.log("update(" + source + ", " + state + ")");
	$.ajax({
		type: "GET",
		url: "api.php?command=update&element=" + element + "&value=" + value,
		async: true
	});
}

function query(){
	$.ajax({
		type: "GET",
        	url: "api.php?command=query",
        	async: true
    	}).complete(function(){
        	setTimeout(function(){ query(); }, 3000);
    	}).success(function(result){
		for(key in result.data) {
			if(result.data[key] == "on") {
				state = true;
			}
			else {
				state = false;
			}

			control = $("[name='lamp-" + key + "']");
			if(control.length != 1) {
				continue;
			}

			control.bootstrapSwitch('state', state, true);
		}
	});
}

