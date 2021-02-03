
# Chip8 Emulator

I wrote a Chip-8 emulator

## Installation

First, clone the repository, then create a build directory and call `cmake`.

Or in other words, run the following lines in your favorite terminal:

```
$ git clone https://github.com/cjordan7/Chip-8-Emulator.git
$ cd path/to/clone/chip8/
$ mkdir build
$ cd build/
$ cmake ..
```


## Use

After building the emulator, launch it with a *ch8 rom*:

```
cd path/to/clone/chip8/build/
$ ./bfInterpreter path/to/chip8.ch8
```




## Keyboard mapping


Original Chip8 Keyboard

|     |     |     |     |
| --- | --- | --- | --- |
| 1   | 2   | 3   | 4   |
| 4   | 5   | 6   | D   |
| 7   | 8   | 9   | E   |
| A   | 0   | B   | F   |


This is the new keyboard:
|     |     |     |     |
| --- | --- | --- | --- |
| 1   | 2   | 3   | 4   |
| Q   | W   | E   | R   |
| A   | S   | D   | F   |
| Z   | X   | C   | V   |
