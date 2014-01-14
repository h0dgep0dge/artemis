var ws = new WebSocket("ws://10.22.33.231:8080");

ws.onopen = function() {
	$("body").fadeIn();
	
	$(".gamebutton").bind("touchstart", function(e) {
		$(this).addClass("pressed");
		e.preventDefault();

		// Get the byte to send
		var code = $(this).data("byte");
		ws.send("#" + code + "1");
	})

	$(".gamebutton").bind("touchend", function(e) {
		$(this).removeClass("pressed");

		// Get the byte to send
		var code = $(this).data("byte");
		ws.send("#" + code + "0");
	})
}

ws.onerror = function() {
	alert("An error occoured connecting to the websocket server");
}