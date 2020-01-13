# Build

## Dependencies

- 2D rendering [SDL2, SDL2_image](https://www.libsdl.org/download-2.0.php), install with OS package manager
- ECS [skipjack/entt](https://github.com/skypjack/entt), included with the sources
- Json parsing [nhlomann/json](https://github.com/nlohmann/json), included with the sources

## CMake

$ mkdir build && cd build
$ cmake ..
$ make

## Redo

I am using [this]() implementation on OSX and [this one]() on Linux

OSX:
$ redo

Linux:
$ redux redo

## Running

After a successful build, run the executable from the project root (not the build directory), in order for it to find the resources.
