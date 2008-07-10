@echo off
rem cls
del lz77_gcc.exe
g++ -Wextra -O3 -I../../.. -DPATL_GCC lz77.cpp -o lz77_gcc.exe
strip lz77_gcc.exe
