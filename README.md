# VexFlowCPP

This project is a command-line utility to render
[VexFlow](https://github.com/vexflow/vexflow) scores off-line. It is designed to
be as lightweight as possible and to support as many platforms as possible. In
particular it aims to be usable by games that don't want to include V8 or a
browser.

## Technology

The idea is to use [QuickJS](https://bellard.org/quickjs/) as the JavaScript
interpreter. The main program is a C++ application that integrates QuickJS with
various simple in-memory graphics drawing functions, then stubs out `document`
and `CanvasRenderingContext2D` to call the right graphics operations.

For convenience, the following pieces are all statically linked/included in the
command line utility:
* QuickJS
* Necessary fonts (Bravura)
* VexFlow distribution file
* JavaScript wrapper code

The goal is that the code can work on any platform, inside any game engine or
GUI system.
