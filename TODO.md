# TODO list

## Threads library

- Async jobs manager
- Concurrency support

## Doc

- Add doxygen comment for the public API,
- Write spritesheet generator readme,

## Level environment sprites

- Map format (json?),
- Map generator,

## Command queue

- Restructure commands execution code in `sz_character.c`
- `Shift` key support to enqueue commands,
- `S` key support to flush command queue,
- `A` key fix,

## Attack logic

- Add distance metering to move and then attack,
- Fix sprite orientation depending on the target,

## Units and groups

- Add units selection with mouse frame,
- Add groups bindings,

## Textures

- Load spritesheet only once per action,

## Lua

- Add lua support,
- Add script engine for user code execution,
