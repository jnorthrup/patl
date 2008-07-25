@echo off
rem cls
del ..\..\release\usort3_gcc.exe
g++ -Wextra -O3 -I../../.. -DPATL_GCC usort3.cpp -o ..\..\release\usort3_gcc.exe
strip ..\..\release\usort3_gcc.exe
