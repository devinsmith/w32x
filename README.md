# w32x
A Win32 inspired API/wrapper over Xlib/X11 for Linux and Unix machines.

# Requirements
Debian based systems require the following libraries:
```
sudo apt-get install libx11-dev libxft-dev
```

# Building
You can use cmake to build the static library and tests.
```
mkdir build
cd build
cmake ..
make
```
This will create a libw32x.a library and build a sample app
that utilizes the library.

Alternatively, Makefiles are provided but may need to be edited for your
platform. Simply change to the src directory and execute make.
This will build a static library called libw32x.a.
You can then change into the test directory to build a sample
app that utilizes the library.


