#!/usr/local/bin/coffee

es = require "execSync"
ws = new (require("ws").Server)
	port: 8080

keyMap = {
	u: "Up"
	b: "Up"
	d: "Down"
	l: "Left"
	r: "Right"
	a: "space"
}

ws.on "connection", (sock) ->
	sock.on "message", (msg, data) ->
		console.log msg
		if msg[0] is "#"
			key = msg[1]
			cmd = msg[2]

			if key of keyMap
				if cmd is "0"
					es.run "xdotool keyup #{keyMap[key]}"
				if cmd is "1"
					es.run "xdotool keydown #{keyMap[key]}"
			else 
				console.log "Can't find #{key}"