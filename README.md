# MBR-Snake

An incomplete game of Snake, that fits into a boot sector on x86, written in C++.

# Building:

Use `make` to build the project. The resulting binary (exactly 512 bytes, including the MBR signature) will be created in the `bin` directory.
If you wish to build it without the MBR signature, just use `make nosig` which only puts the executable code itself in the file.

# Running:

After building it, run `copy.sh` with a destination in order to copy the binary to a device. For example `./copy.sh /dev/sdb` will copy the game into the boot sector of `/dev/sdb`.

# Usage:

After booting into the game, just use the arrow keys to change the direction of the snake and collect the food.

Sadly I couldn't cram all of the game logic into 510 bytes using C++, so the snake doesn't grow. Apart from that, most of the game is implemented, such as the food respawning in a random location after you collect it, or the snake being reset when going out of the playing area.

# Notes:

The source code is all in a single file which makes it a tad hard to read, but I had to do that for optimization/code size reasons. With multiple translation units, the compiler might not have been able to optimize the code in the same way.

_____________________
Licensed under WTFPL v2 (see the file `COPYING`).