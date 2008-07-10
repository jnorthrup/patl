@echo off
rem cls
del word_suffix_gcc.exe
g++ -Wextra -O3 -I../../.. -DPATL_GCC word_suffix.cpp -o word_suffix_gcc.exe
strip word_suffix_gcc.exe
