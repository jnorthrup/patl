@echo off
rem cls
del multi_search_gcc.exe
g++ -Wextra -O3 -I../../.. -DPATL_GCC multi_search.cpp -o multi_search_gcc.exe
strip multi_search_gcc.exe
