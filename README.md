# sha256

### A sha256 implementation in C

This implementation uses C and gcc without any external libraries. Only `itoa_c()` and `little_endian()` came from outside sources. GCC is not the only compiler that works with this program, this program also works with clang.

### How to compile

Check for gcc

`gcc --version`

Setup for linux/wsl

`sudo apt update`

`sudo apt install build-essential`

`sudo apt-get install manpages-dev` (optional)

`make`

Setup for windows

Download mingw-get-setup.exe at `https://sourceforge.net/projects/mingw/files/Installer/mingw-get-setup.exe/download`

`Run the exe and follow installation steps`

`make`

Setup for mac

Download developer tools via Apple's Xcode or Apple's website at `https://developer.apple.com/develop/`

`make`

### Include in Project

Write `#include "sha256.h"` or `#include "path/to/sha256.h"` (depending on how you are using it)

The include at the top of `sha256.c` should also be adjusted accordingly to your use case.

However, if `sha256.c` and `sha256.h` are in the same directory, the include in `sha256.c` shouldn't need to be modified.

### Notes

Clang be used alternatively and run `make clang` instead

Any lines with `find . -type f -exec touch {} +` can be removed

Variables at the top of the makefile can be adjusted to your preferences

Legacy just holds previous versions of my sha256 code

I am on windows and I use WSL to get access to POSIX functions (see disclaimer for more information)

### Disclaimer

I have not tested any of these programs on a mac

On windows (and maybe mac) `getdelim()` is not available becuase `getdelim()` is a POSIX function. An alternative is to add `#define _GNU_SOURCE` above `#include <stdio.h>`, but this may not work (still worth a shot).
