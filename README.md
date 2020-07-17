# Online RPG Project

### This repository holds the backend of the project


#### Instructions

This project uses **conan** for package management and **premake5** for generating project files. Make sure you have the `conan` command on you PATH.

After you clone the repo, run the following command:

```
premake5 <action>
```

Where "action" is any of the values in [this table](https://github.com/premake/premake-core/wiki/Using-Premake#using-premake-to-generate-project-files), for example `.\premake5 vs2019` will run conan install and generate Visual Studio 2019 project files.

You can also specify the build configuration ("release" or "debug") by running:

```
premake5 --build=<type> <action>
```

#### Tests

To run tests, build and run the `test` project.


#### TODO

Add premake5 action for automatically building and running tests on Windows and Linux