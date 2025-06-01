# SadZombik's Sprite Engine

## Sprite storage

To use the engine you have to create `res` folder in the root directory or in the build directory
with your characters, their spritesheets and text file with the amount of sprites in the
spritesheet.

```
res/
|--Char1/
|----Actions/
|------Action1/
|--------sheet.png
|--------count.txt
|------Action2/
|--------sheet.png
|--------count.txt
|--Char2/
...
```

Sprite loading functions will search for `sheet.png` and `count.txt` in the specified action
directory.

## Build

```
mkdir build
cmake -S . -B build
cmake --build build
```
