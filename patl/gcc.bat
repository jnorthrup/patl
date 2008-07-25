@echo off
rem cls
del ..\release\patcont_gcc.exe
g++ -Wextra -O3 -I../.. -DPATL_GCC patcont.cpp -o ..\release\patcont_gcc.exe
strip ..\release\patcont_gcc.exe
