@echo off
rem cls
del insider_gcc.exe
g++ -Wextra -O3 -I../../.. -DPATL_GCC ./insider.cpp -o insider_gcc.exe
strip insider_gcc.exe
