@echo off
rem cls
del ..\..\release\maxpals_gcc.exe
g++ -Wextra -O3 -I../../.. -DPATL_GCC maxpals.cpp -o ..\..\release\maxpals_gcc.exe
strip ..\..\release\maxpals_gcc.exe
