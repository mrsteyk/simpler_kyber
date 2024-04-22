// We live in a fucking society
// KeccakP-1600-times4-SIMD256.o
#include "../../common/keccak4x/KeccakP-1600-times4-SIMD256.c"

// Society
#include "../../common/fips202.c"
#include "../../common/randombytes.c"

#undef cdecl
#undef small

#include "cbd.c"
#include "consts.c"
#include "fips202x4.c"
#include "indcpa.c"
#include "kem.c"
#include "poly.c"
#include "polyvec.c"
#include "rejsample.c"
#include "symmetric-shake.c"
#include "verify.c"
