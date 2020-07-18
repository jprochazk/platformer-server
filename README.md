# platformer-server

This repository holds the backend of the project, for the front-end, see [this repository](https://github.com/jprochazk/platformer-client).

### Goals

Write a game server with the following features:

* Authentication (one-time JWT)
* Persistence (PostgreSQL)
* ECS
* LUA scripting
* JSON/CBOR serialization

And build a 2d platformer in it.

##### Requirements

* Compiler that can do C++17
    * Clang/LLVM 5+
    * MSVC 14.11+ / Visual Studio 2017+
    * Xcode 10+
    * GCC 9+
* CMake 3.17+
* Conan 1.26.0+

#### Instructions

This project uses **conan** for package management and **cmake** for generating project files or building. Make sure you have both installed on your system.

To build on Linux, simply run `build.sh`. This will generate a `server` executable in `build/bin`, which you can run by doing `build/bin/server`.


#### TODO

* Write more info about the project in this readme file
* Use CMake for unit testing.