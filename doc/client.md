# Protocol Documentation

The connection to the server is made by WebSocket on port 8080.

Every message is JSON encoded. There is currently only one type of message:

    {
      "type": "key",
      "keycode": a single character representing a key,
      "state": "up" or "down"
    }

Possible keycodes are as follows:

* `a` (A)
* `b` (B)
* `x` (X)
* `y` (Y)
* `u` (↑)
* `d` (↓)
* `l` (←)
* `r` (→)
* `t` (Start)
* `e` (Select)

This is based on the SNES controller.
