# sha256

### A sha256 implementation in C

This implementation uses C and gcc without any external libraries. Only `itoa_c()` and `little_endian()` came from outside sources. GCC is not the only compiler that works with this program, this program also works with clang.

### How to use

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

### Notes

Clang be used alternatively and run `make clang` instead

Any lines with `find . -type f -exec touch {} +` can be removed

Variables at the top of the makefile can be adjusted to your preferences
