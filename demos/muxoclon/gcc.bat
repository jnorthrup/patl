@echo off
rem cls
del ..\..\release\muxoclon_gcc.exe
g++ -Wextra -O3 -I../../.. -DPATL_GCC ./muxoclon.cpp -o ..\..\release\muxoclon_gcc.exe
strip ..\..\release\muxoclon_gcc.exe
