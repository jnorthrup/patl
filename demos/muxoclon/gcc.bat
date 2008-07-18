@echo off
rem cls
del muxoclon_gcc.exe
g++ -Wextra -O3 -I../../.. -DPATL_GCC ./muxoclon.cpp -o muxoclon_gcc.exe
strip muxoclon_gcc.exe
