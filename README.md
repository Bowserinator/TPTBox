# The Powder Box

The Powder Box is a 3D falling sand game built on top of the [Raylib engine](https://www.raylib.com/), inspired primarily by The Powder Toy. This project is very much a work in progress!

Join our discord: [https://discord.gg/BJzM5hreKT](https://discord.gg/BJzM5hreKT)

## Build Instructions

This project is based on the [Raylib premake template](https://github.com/raylib-extras/game-premake), which contains the specific build instructions. An abridged version:

```bash
# Pick a command below depending on your OS
./premake5 gmake2        # Linux
premake-mingw.bat        # MinGW (Windows)
./premake5.osx gmake2    # MacOS
premake-VisualStudio.bat # Visual Studio project, untested! (Windows)

# Debug build (default):
make config=debug_x64

# Release build:
make config=release_x64
```

Executables will be in the `_bin` folder. Note that you will need to have a compiler that supports `OpenMP`, `C++20` as well as `opengl4.3`. If make fails to detect your compiler, try specifying the compiler directly, ie `make CC=g++`.


## Licenses & Credits

This code borrows great work by other members of the open-source community. Licenses for portions of the code can be found in the `LICENSES/` folder. Notably,

- Francis Engelmann for [his 3D DDA algorithm](https://github.com/francisengelmann/fast_voxel_traversal) (MIT)
- The Powder Toy for some implementation details
- Jeffery Myers and Ramon Santamaria (@raysan5) for the [Raylib premake project](https://github.com/raylib-extras/game-premake) (zlib)

This project is licensed under the GPLv3 license.
