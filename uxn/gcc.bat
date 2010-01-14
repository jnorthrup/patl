@echo off
if not exist release md release

echo ---
echo     building patl
echo ---
del release\patl_gcc.exe
g++ -Wextra -O3 -I.. patl/patcont.cpp -o release\patl_gcc.exe
strip release\patl_gcc.exe

echo ---
echo     building ins_test
echo ---
del release\ins_test_gcc.exe
g++ -Wextra -O3 -I.. demos/ins_test/ins_test.cpp -o release\ins_test_gcc.exe
strip release\ins_test_gcc.exe

echo ---
echo     building insider
echo ---
del release\insider_gcc.exe
g++ -Wextra -O3 -I.. demos/insider/insider.cpp -o release\insider_gcc.exe
strip release\insider_gcc.exe

echo ---
echo     building levenshtein
echo ---
del release\levenshtein_gcc.exe
g++ -Wextra -O3 -I.. demos/levenshtein/levenshtein.cpp -o release\levenshtein_gcc.exe
strip release\levenshtein_gcc.exe

echo ---
echo     building lz77
echo ---
del release\lz77_gcc.exe
g++ -Wextra -O3 -I.. demos/lz77/lz77.cpp -o release\lz77_gcc.exe
strip release\lz77_gcc.exe

echo ---
echo     building maxpals
echo ---
del release\maxpals_gcc.exe
g++ -Wextra -O3 -I.. demos/maxpals/maxpals.cpp -o release\maxpals_gcc.exe
strip release\maxpals_gcc.exe

echo ---
echo     building multi_align
echo ---
del release\multi_align_gcc.exe
g++ -Wextra -O3 -I.. demos/multi_align/multi_align.cpp -o release\multi_align_gcc.exe
strip release\multi_align_gcc.exe

echo ---
echo     building multi_search
echo ---
del release\multi_search_gcc.exe
g++ -Wextra -O3 -I.. demos/multi_search/multi_search.cpp -o release\multi_search_gcc.exe
strip release\multi_search_gcc.exe

echo ---
echo     building muxoclon
echo ---
del release\muxoclon_gcc.exe
g++ -Wextra -O3 -I.. demos/muxoclon/muxoclon.cpp -o release\muxoclon_gcc.exe
strip release\muxoclon_gcc.exe

echo ---
echo     building sort_lin
echo ---
del release\sort_lin_gcc.exe
g++ -Wextra -O3 -I.. demos/sort_lin/sort_lin.cpp -o release\sort_lin_gcc.exe
strip release\sort_lin_gcc.exe

echo ---
echo     building usort3
echo ---
del release\usort3_gcc.exe
g++ -Wextra -O3 -I.. demos/usort3/usort3.cpp -o release\usort3_gcc.exe
strip release\usort3_gcc.exe

echo ---
echo     building word_suffix
echo ---
del release\word_suffix_gcc.exe
g++ -Wextra -O3 -I.. demos/word_suffix/word_suffix.cpp -o release\word_suffix_gcc.exe
strip release\word_suffix_gcc.exe
