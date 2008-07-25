@echo off
if not exist release md release
cd patl
call gcc.bat
cd ..\demos
call gcc.bat
