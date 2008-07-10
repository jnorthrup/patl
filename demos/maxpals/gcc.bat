@echo off
rem cls
del maxpals_gcc.exe
g++ -Wextra -O3 -I../../.. -DPATL_GCC maxpals.cpp -o maxpals_gcc.exe
strip maxpals_gcc.exe
