@echo off
rem cls
del patcont_gcc.exe
g++ -Wextra -O3 -I../.. -DPATL_GCC patcont.cpp -o patcont_gcc.exe
strip patcont_gcc.exe
