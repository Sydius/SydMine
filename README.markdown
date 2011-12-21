SydMine
=======

SydMine is an unfinished Minecraft server written in C++ as a hobby
project to try and create a more efficient and custom Minecraft server
that uses less RAM and is more difficult to cheat against (X-ray mods
etc.). This is only a prototype.

Unfortunately, some decisions in the design of the Minecraft protocol
inhibit control over much of the behavior of the client, such as the
placement of biomes (which are entirely calculated on the client
with no input from the server beyond the seed). This, along with
the fragility of the protocol itself has lead me to put this project
on hold.

The basic networking layer is in (albeit messy) and you can log in,
walk around, and destroy blocks. Nothing else works.
