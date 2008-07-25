@echo off
rem cls
del ..\..\release\lz77_gcc.exe
g++ -Wextra -O3 -I../../.. -DPATL_GCC lz77.cpp -o ..\..\release\lz77_gcc.exe
strip ..\..\release\lz77_gcc.exe
