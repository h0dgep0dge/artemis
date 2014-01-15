var ws = new WebSocket("ws://10.22.33.12:8080");

ws.onopen = function() {
	$("body").fadeIn();
}

ws.onerror = function() {
	alert("An error occoured connecting to the websocket server");
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

		if (e.type == "touchstart")
			touches[tid] = false;

		if (elem != null && elem.classList.contains("gamebutton")) {
			var code = elem.dataset.byte;

			if (states[code] == false) {
				states[code] = true;
			}

			if (touches[tid] != elem) {
				if (touches[tid] != false) {
					states[touches[tid].dataset.byte] = code;
					touches[tid].classList.remove("pressed")

					ws.send("#" + touches[tid].dataset.byte + "0");
				}

				ws.send("#" + code + "1");

				touches[tid] = elem;
				elem.classList.add("pressed")
			}
		}
	}
}

document.addEventListener("touchstart", function(e) {
	updateTouches(e);

	e.preventDefault();
})

document.addEventListener("touchmove", function(e) {
	updateTouches(e);

	e.preventDefault();
})

document.addEventListener("touchend", function(e) {
	for (var i=0; i < e.changedTouches.length; i++) {
		var touch = e.changedTouches[i];
		var tid = touch.identifier;

		touches[tid].classList.remove("pressed");

		ws.send("#" + touches[tid].dataset.byte + "0");

		touches[tid] = false;
	}

	e.preventDefault();
})

// $(".gamebutton").bind("touchstart", function(e) {
// 	$(this).addClass("pressed");
// 	e.preventDefault();

// 	// Get the byte to send
// 	var code = $(this).data("byte");
// 	ws.send("#" + code + "1");
// })

// $(".gamebutton").bind("touchend", function(e) {
// 	$(this).removeClass("pressed");

// 	// Get the byte to send
// 	var code = $(this).data("byte");
// 	ws.send("#" + code + "0");
// })
