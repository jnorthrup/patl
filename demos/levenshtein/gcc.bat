@echo off
rem cls
del ..\..\release\levenshtein_gcc.exe
g++ -Wextra -O3 -I../../.. -DPATL_GCC ./levenshtein.cpp -o ..\..\release\levenshtein_gcc.exe
strip ..\..\release\levenshtein_gcc.exe
