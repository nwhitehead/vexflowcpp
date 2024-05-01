# VexFlowCPP

NOTE: This project is abandoned, superceded by [vexflowrust](https://github.com/nwhitehead/vexflowrust).

This project is a command-line utility to render
[VexFlow](https://github.com/vexflow/vexflow) scores off-line. It is designed to
be as lightweight as possible and to support as many platforms as possible. In
particular it aims to be usable by games that don't want to (or can't) include a
JIT compiler like V8 or a full-featured browser.

## Technology

The idea is to use [QuickJS](https://bellard.org/quickjs/) as the JavaScript
interpreter. The main program is a C++ application that integrates QuickJS with
various simple in-memory graphics drawing functions, then stubs out `document`
and `CanvasRenderingContext2D` to call the right graphics operations.

For graphics, this project uses the wonderful [STB](https://github.com/nothings/stb/)
libraries of Sean Barrett. This includes:
* `stb_truetype.h` for font rendering
* `stb_image_write.h` for image writing

For convenience, the following pieces are all statically linked/included in the
command line utility:
* QuickJS
* Necessary fonts (Bravura)
* VexFlow distribution file
* JavaScript wrapper code

The goal is that the code can work on any platform, inside any game engine or
GUI system.

## Building

Build using cmake:

    mkdir build
    cd build
    cmake ..
    make

## Status

The program currently generates a static test image. It is totally broken but building and generating something.
