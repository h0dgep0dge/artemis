var ws = new WebSocket("ws://10.22.33.231:8080");

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