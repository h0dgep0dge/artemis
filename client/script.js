var ws = new WebSocket("ws://10.22.33.13:8080");

ws.onopen = function() {
	$("body").fadeIn();
}

ws.onerror = function() {
	alert("An error occoured connecting to the websocket server");
}

ws.onclose = function() {
	alert("Connection closed!")
	$("body").hide();
}

ws.sendKey = function(keycode, state) {
	var e = {
		type: "key",
		keycode: keycode,
		state: state
	}

	ws.send(JSON.stringify(e));
}

var touches = {};
var states = {};
var buttons = $(".gamebutton");

for (var i = 0; i < buttons.length; i++) {
	var button = buttons[i];
	var code = button.dataset.byte;

	states[code] = false;
}

function updateTouches(e) {
	for (var i=0; i < e.changedTouches.length; i++) {
		var touch = e.changedTouches[i];
		var tid = touch.identifier;
		var elem = document.elementFromPoint(touch.pageX, touch.pageY);

		if (e.type === "touchstart")
			touches[tid] = false;

		if (elem != null && elem.classList.contains("gamebutton")) {
			var code = elem.dataset.byte;

			if (states[code] === false) {
				states[code] = true;
			}

			if (touches[tid] != elem) {
				if (touches[tid] != false) {
					states[touches[tid].dataset.byte] = code;
					touches[tid].classList.remove("pressed")

					ws.sendKey(touches[tid].dataset.byte, "up");
				}

				ws.sendKey(code, "down");

				touches[tid] = elem;
				elem.classList.add("pressed")
			}
		}
	}
}

// document.addEventListener("touchstart", function(e) {
// 	updateTouches(e);

// 	e.preventDefault();
// })

// document.addEventListener("touchmove", function(e) {
// 	updateTouches(e);

// 	e.preventDefault();
// })

// document.addEventListener("touchend", function(e) {
// 	for (var i=0; i < e.changedTouches.length; i++) {
// 		var touch = e.changedTouches[i];
// 		var tid = touch.identifier;

// 		touches[tid].classList.remove("pressed");

// 		ws.sendKey(touches[tid].dataset.byte, "up");

// 		touches[tid] = false;
// 	}

// 	e.preventDefault();
// })
