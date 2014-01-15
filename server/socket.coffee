#!/usr/local/bin/coffee

ws = new (require("ws").Server)
	port: 8080

ws.on "connection", (sock) ->
	sock.on "message", (msg, data) ->
		process.stdout.write data.buffer