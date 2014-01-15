var ws = new (require("ws").Server)({
	port: 8080
});

var players = {};

function nextPlayerNum() {
	for (var i = 1;;i++) {
		if (!(i in players))
			return i;
	}
}

ws.on("connection", function(sock) {
	sock.playerNum = nextPlayerNum();

	players[sock.playerNum] = sock;
	console.error(Object.keys(players));

	sock.on("message", function(msg, data) {
		// Parse event JSON
		var e = JSON.parse(msg);

		// Check event type
		if (e.type == "key") {
			// Write event info to C program

			process.stdout.write (
				"#" +
				e.keycode +
				(e.state == "up" ? "0" : "1") +
				sock.num
			);
		}
	});

	sock.on("error", function() {
		console.error("SOCK ERROR", arguments)
	})

	sock.on("close", function() {
		delete players[sock.playerNum];
		console.error(Object.keys(players));
	});
});
