@echo off
:: REQUIRES Mingw
:: "WHERE" is not supported by wine-staging 6.2 as of Feb 21 2021, No testing could be performed
WHERE g++
IF %ERRORLEVEL% NEQ 0 (echo "Please install mingw for x86 before running this script") else (g++ -Wall -m32 -std=c++17 -DWIN32 testProgram.cpp -o test.exe && .\test.exe)
PAUSE
