Object.size = function(obj) {
	var size = 0, key;
	for (key in obj) {
		if (obj.hasOwnProperty(key)) size++;
	}
	return size;
};

var ws = new (require("ws").Server)({
	port: 8080
});

var players = {};

var pin = process.argv[0];

function nextPlayerNum() {
	for (var i = 1;;i++) {
		if (!(i in players))
			return i;
	}
}

ws.on("connection", function(sock) {
	if (Object.size(players) > 9) {
		sock.close()
	}

	sock.authenticated = false;
	
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
				sock.playerNum + "\n"
			);
		}
	});

	sock.on("error", function() {
		console.error("Socket error!", arguments)
	})

	sock.on("close", function() {
		delete players[sock.playerNum];
	});
});
