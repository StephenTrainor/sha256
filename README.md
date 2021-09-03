# sha256

### A sha256 implementation in C

This implementation uses C without any external libraries. Only `little_endian()` came from outside sources. GCC is not the only compiler that works with this program, this program also works with clang. By default, running `make` will use gcc. This can be changed by editing the makefile or running `make clang`.

### How to compile

Check for gcc

`gcc --version`

or for clang

`clang --version`


Setup for linux/wsl

`sudo apt update`

`sudo apt install build-essential`

`sudo apt-get install manpages-dev` (optional)

`make`


Setup for windows

Download mingw-get-setup.exe at `https://sourceforge.net/projects/mingw/files/Installer/mingw-get-setup.exe/download`

Run the exe and follow installation steps

`make windows`


Setup for mac

Download developer tools via Apple's Xcode or Apple's website at `https://developer.apple.com/develop/`

`make`

### Include in Project

Write `#include "sha256.h"` or `#include "path/to/sha256.h"` (depending on how you are using it)

The include at the top of `sha256.c` should also be adjusted accordingly to your use case.

However, if `sha256.c` and `sha256.h` are in the same directory, the include in `sha256.c` doesn't need to be modified.

### Message Digest

The Message Digest (output) is printed out in hexadecimal. To verify that my sha256 implementation works, I created some simple tests.

Run `make tests` or `make tets_windows` accordingly to use sha256.c

Run `python tests.py` or `python3 tests.py` accordingly to use Python's hashlib.

Run `python -V` or `python3 -V` to check if you have python installed

### .So

sha256() can be compiled into a .so, but only on UNIX machines. 

Use `make so` for linux/WSL or `make so_mac` for mac.

Load .so with `CDLL()` from `ctypes` as seen in `so.py`

Run with python2, python3 is not supported yet.

### Notes

Any lines with `find . -type f -exec touch {} +` can be removed (from the makefile)

Variables at the top of the makefile can be adjusted to your preferences

Legacy just holds previous versions of my sha256 code

In terms of optimizations, I use -O3 when compiling (sometimes -O2), `__restrict/restrict` when possible, and inlining the small functions. I am currently looking into removing branches in the code and using various bit hacks to make things go faster.

### Disclaimer

I have not tested any of these programs on a mac

For `posix_sha256.c` only:
    `posix_sha256.c` relies on a POSIX function `getdelim()` that is not accessible on windows machines, even with `#define _GNU_SOURCE`. 

There are various indentation anomalies, but many of them are hard to get rid (tabs and spaces on my editor and github).

Use `sha256.c` at your own risk, I created this function and may not be completely safe.
