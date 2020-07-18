# platformer-server

This repository holds the backend of the project, for the front-end, see [this repository](https://github.com/jprochazk/platformer-client).

### Goals

Write a game server and build a 2d platformer in it.

Currently, the server has the following features:

* Networking (WebSockets)
* JSON/CBOR serialization
* PostgreSQL database connection with asynchronous queries
    * Currently implemented using boost::async, will be implemented using boost.asio in the future
* JSON configuration files
* Exception-safe packet handling
* AABB collision detection + resolution
* Game state synchronization

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