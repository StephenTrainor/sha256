from ctypes import *

sha = CDLL('./sha256.so')

sha.sha256("texts/abc.txt")
