@echo off
rem cls
del ..\..\release\multi_search_gcc.exe
g++ -Wextra -O3 -I../../.. -DPATL_GCC multi_search.cpp -o ..\..\release\multi_search_gcc.exe
strip ..\..\release\multi_search_gcc.exe
