@echo off
rem cls
del ..\..\release\insider_gcc.exe
g++ -Wextra -O3 -I../../.. -DPATL_GCC ./insider.cpp -o ..\..\release\insider_gcc.exe
strip ..\..\release\insider_gcc.exe
