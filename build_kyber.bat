@echo off

pushd build
set KYBER_CFLAGS=-O3 -g -Wall -Wextra -Wpedantic -Werror -Wmissing-prototypes -Wredundant-decls -std=c99 -I../pqclean/common %EXTRA_CFLAGS%

if "%KYBER768_CLEAN%" == "1" (
echo Building kyber768 clean
clang %KYBER_CFLAGS% ../pqclean/kyber768/clean/amalgamated.c -c -o kyber768_clean.o
)

REM

if "%KYBER768_AVX2%" == "1" (
echo Building kyber768 AVX2 WIP
clang -mavx2 -mbmi2 -mpopcnt %KYBER_CLFAGS% -Wpointer-arith -Wshadow -Wno-keyword-macro ../pqclean/kyber768/avx2/amalgamated.c -c -o kyber768_avx2_c.o || exit /b 1
clang -mavx2 -mbmi2 -mpopcnt %KYBER_CLFAGS% -Wpointer-arith -Wshadow -I../pqclean/kyber768/avx2 ../pqclean/kyber768/avx2/basemul.S -c -o kyber768_avx2_basemul.o || exit /b 1
clang -mavx2 -mbmi2 -mpopcnt %KYBER_CLFAGS% -Wpointer-arith -Wshadow -I../pqclean/kyber768/avx2 ../pqclean/kyber768/avx2/fq.S -c -o kyber768_avx2_fq.o || exit /b 1
clang -mavx2 -mbmi2 -mpopcnt %KYBER_CLFAGS% -Wpointer-arith -Wshadow -I../pqclean/kyber768/avx2 ../pqclean/kyber768/avx2/invntt.S -c -o kyber768_avx2_invntt.o || exit /b 1
clang -mavx2 -mbmi2 -mpopcnt %KYBER_CLFAGS% -Wpointer-arith -Wshadow -I../pqclean/kyber768/avx2 ../pqclean/kyber768/avx2/ntt.S -c -o kyber768_avx2_ntt.o || exit /b 1
clang -mavx2 -mbmi2 -mpopcnt %KYBER_CLFAGS% -Wpointer-arith -Wshadow -I../pqclean/kyber768/avx2 ../pqclean/kyber768/avx2/shuffle.S -c -o kyber768_avx2_shuffle.o || exit /b 1
llvm-ar -r kyber768_avx2.a kyber768_avx2_c.o kyber768_avx2_basemul.o kyber768_avx2_fq.o kyber768_avx2_invntt.o kyber768_avx2_ntt.o kyber768_avx2_shuffle.o
del kyber768_avx2_*.o || exit /b 1
)

REM
if "%KYBER768_ARCH64%" == "1" (
echo Building kyber768 AARCH64 WIP
)

popd