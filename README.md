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
* Packet handling
* AABB collision detection + resolution
* Game state synchronization

##### Requirements

* Compiler that can do C++17
    * Clang/LLVM 5+
    * MSVC 14.11+ / Visual Studio 2017+
    * Xcode 10+
    * GCC 9+
* Conan 1.26.0+

#### Instructions

This project uses **conan** for package management and **premake** for generating project files.

To build the project

on linux:
```
$ ./configure.sh <ACTION> [--build=BUILD_TYPE]
```

on windows:
```
> .\configure.bat <ACTION> [--build=BUILD_TYPE]
```

Where `ACTION` is any of the actions listed [here](https://github.com/premake/premake-core/wiki/Using-Premake#using-premake-to-generate-project-files), and `BUILD_TYPE` is `release` (default) or `debug`.

For example, to generate Unix makefiles, build, and run the executable on Linux, you would do:
```
$ chmod +x configure.sh
$ chmod +x premake/premake5
$ ./configure.sh gmake
$ make -j 8
$ bin/server
```

To run on Windows (visual studio 2019), you would:
```
> .\configure.bat vs2019
```
Then open the generated .sln file, and press F5 to build and run.+

#### TODO

* Write more info about the project in this readme file
* Run tests using doctest + premake