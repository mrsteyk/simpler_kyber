@echo off

REM Thank you, Ryan "rjf" Fleury

setlocal
cd /d "%~dp0"

for %%a in (%*) do set "%%a=1"

if not "%KYBER768_CLEAN%" == "1" if not "%KYBER768_AVX2%" == "1" if not "%KYBER768_AARCH64%" == "1" set KYBER768_CLEAN=1

if not exist build mkdir build

call build_kyber.bat

if not "%KYBER768_AVX2%" == "1" set KYBER768_AVX2=0
if not "%KYBER768_AARCH64%" == "1" set KYBER768_AARCH64=0

REM Linking options

set LINK_OPTS=-I../pqclean/common
if "%KYBER768_AVX2%" == "1" set LINK_OPTS=kyber768_avx2.a

echo Link options %LINK_OPTS%

pushd build
echo Building main
REM O2 is WAY less aggressive as O3 and doesn't try to aggressively produce vector-aligned memcpy's and shit.
clang -gcodeview -fdiagnostics-absolute-paths -DNDEBUG -fuse-ld=lld -flto -fno-exceptions -fno-unwind-tables -fno-asynchronous-unwind-tables -march=native -mtune=native -std=c11 -O2 -g -I../pqclean ../src/main.c %LINK_OPTS% -o simpler_kyber.exe -lAdvapi32 -DKYBER768_AVX2=%KYBER768_AVX2% -DKYBER768_AARCH64=0
popd
