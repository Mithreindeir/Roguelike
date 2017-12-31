# Roguelike
Prototype for a ascii roguelike written in C with ncurses. Featuring procedural generated maps, fov calculations, A* and more.

![Screenshot](https://raw.githubusercontent.com/mithreindeir/roguelike/master/screensht.png)

## Building
If you have not yet installed ncurses then install it.
(On debian based distros you can type)

    sudo apt-get install libncurses-dev

Then to compile

    gcc -o roguelike *.c -lm -lncurses -w

## Notes
The menu is just a mock up, it is not functional. This is intended as a tech demo/prototype of a roguelike to demonstate several common techniques and not actually meant to be a game.
