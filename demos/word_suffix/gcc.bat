@echo off
rem cls
del ..\..\release\word_suffix_gcc.exe
g++ -Wextra -O3 -I../../.. -DPATL_GCC word_suffix.cpp -o ..\..\release\word_suffix_gcc.exe
strip ..\..\release\word_suffix_gcc.exe
