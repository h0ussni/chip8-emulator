## Chip8 Emulator

An emulator of the chip8 processor which takes chip8 rom files and runs them as example the old skool pong game. Tested on macOS and Linux.

### Requirements
- pkg-config
- [SDL2](https://www.libsdl.org/index.php) graphics library

### Installation
    make -B

### Usage
    $ ./chip8emu <filepath.ch8>
To start in fullscreen add the `-f` flag at the end.
