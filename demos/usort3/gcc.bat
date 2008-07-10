@echo off
rem cls
del usort3_gcc.exe
g++ -Wextra -O3 -I../../.. -DPATL_GCC usort3.cpp -o usort3_gcc.exe
strip usort3_gcc.exe
