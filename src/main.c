#include <stdio.h>
#include <string.h>

#include <stdint.h>

// NOTE(mrsteyk): Inline api.h
// NOTE(mrsteyk): If those sizes were inconsistent that would've been a fucking spec violation lol
#define PQCLEAN_KYBER768_CLEAN_CRYPTO_SECRETKEYBYTES  2400
#define PQCLEAN_KYBER768_CLEAN_CRYPTO_PUBLICKEYBYTES  1184
#define PQCLEAN_KYBER768_CLEAN_CRYPTO_CIPHERTEXTBYTES 1088
#define PQCLEAN_KYBER768_CLEAN_CRYPTO_BYTES           32

#if KYBER768_AVX2
int PQCLEAN_KYBER768_AVX2_crypto_kem_keypair(uint8_t *pk, uint8_t *sk);
int PQCLEAN_KYBER768_AVX2_crypto_kem_enc(uint8_t *ct, uint8_t *ss, const uint8_t *pk);
int PQCLEAN_KYBER768_AVX2_crypto_kem_dec(uint8_t *ss, const uint8_t *ct, const uint8_t *sk);

#define kyber768_crypto_kem_keypair(pk, sk) PQCLEAN_KYBER768_AVX2_crypto_kem_keypair(pk, sk)
#define kyber768_crypto_kem_enc(ct, ss, pk) PQCLEAN_KYBER768_AVX2_crypto_kem_enc(ct, ss, pk)
#define kyber768_crypto_kem_dec(ss, ct, sk) PQCLEAN_KYBER768_AVX2_crypto_kem_dec(ss, ct, sk)

#define KYBER768_PROVIDER "AVX2"
#elif KYBER768_AARCH64
#error "AARCH64"
#else
int PQCLEAN_KYBER768_CLEAN_crypto_kem_keypair(uint8_t *pk, uint8_t *sk);
int PQCLEAN_KYBER768_CLEAN_crypto_kem_enc(uint8_t *ct, uint8_t *ss, const uint8_t *pk);
int PQCLEAN_KYBER768_CLEAN_crypto_kem_dec(uint8_t *ss, const uint8_t *ct, const uint8_t *sk);

#define kyber768_crypto_kem_keypair(pk, sk) PQCLEAN_KYBER768_CLEAN_crypto_kem_keypair(pk, sk)
#define kyber768_crypto_kem_enc(ct, ss, pk) PQCLEAN_KYBER768_CLEAN_crypto_kem_enc(ct, ss, pk)
#define kyber768_crypto_kem_dec(ss, ct, sk) PQCLEAN_KYBER768_CLEAN_crypto_kem_dec(ss, ct, sk)
#define KYBER768_PROVIDER "CLEAN"
#endif

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>
#include <intrin.h>

enum {
    MEASURE_KP = 0,
    MEASURE_ENC,
    MEASURE_DEC,
    
    MEASURE_COUNT,
};

typedef struct {
    uint64_t tsc;
    LARGE_INTEGER pc;
} Perf;

typedef struct {
    Perf start;
    Perf end;
} Measurement;

static void
perf_measure(Perf* a) {
    a->tsc = __rdtsc();
    QueryPerformanceCounter(&a->pc);
}

static void
measurement_print(Measurement a, LARGE_INTEGER PC_FREQ) {
    uint64_t tsc = a.end.tsc - a.start.tsc;
    uint64_t pc = a.end.pc.QuadPart - a.start.pc.QuadPart;
    printf("%.02fms %llucy", pc*1000/(float)PC_FREQ.QuadPart, tsc);
}

int main() {
    Measurement m[MEASURE_COUNT];
    LARGE_INTEGER freq;
    QueryPerformanceFrequency(&freq);
    
    printf(KYBER768_PROVIDER "\nSKB %d\nPKB %d\nCTB %d\nB %d\n",
           PQCLEAN_KYBER768_CLEAN_CRYPTO_SECRETKEYBYTES,
           PQCLEAN_KYBER768_CLEAN_CRYPTO_PUBLICKEYBYTES,
           PQCLEAN_KYBER768_CLEAN_CRYPTO_CIPHERTEXTBYTES,
           PQCLEAN_KYBER768_CLEAN_CRYPTO_BYTES
           );
    
    uint8_t sk[PQCLEAN_KYBER768_CLEAN_CRYPTO_SECRETKEYBYTES];
    uint8_t pk[PQCLEAN_KYBER768_CLEAN_CRYPTO_PUBLICKEYBYTES];
    
    // A creates assymetric keypair
    perf_measure(&m[MEASURE_KP].start);
    kyber768_crypto_kem_keypair(pk, sk);
    perf_measure(&m[MEASURE_KP].end);
    
    // A sends pk
    // B derives ssk and forms a response
    uint8_t ct[PQCLEAN_KYBER768_CLEAN_CRYPTO_CIPHERTEXTBYTES];
    uint8_t key_b[PQCLEAN_KYBER768_CLEAN_CRYPTO_BYTES];
    perf_measure(&m[MEASURE_ENC].start);
    kyber768_crypto_kem_enc(ct, key_b, pk);
    perf_measure(&m[MEASURE_ENC].end);
    
    printf("Bob's Session Key:");
    for (size_t i = 0; i < sizeof(key_b); i++) {
        printf(" %02hhX", ((uint8_t*)key_b)[i]);
    }
    printf("\n");
    
    // B sends ct?
    // A gets ssk as well
    uint8_t key_a[PQCLEAN_KYBER768_CLEAN_CRYPTO_BYTES];
    perf_measure(&m[MEASURE_DEC].start);
    kyber768_crypto_kem_dec(key_a, ct, sk);
    perf_measure(&m[MEASURE_DEC].end);
    
    printf("Timings:\n");
    printf("KP: ");
    measurement_print(m[MEASURE_KP], freq);
    printf("\n");
    printf("ENC: ");
    measurement_print(m[MEASURE_ENC], freq);
    printf("\n");
    printf("DEC: ");
    measurement_print(m[MEASURE_DEC], freq);
    printf("\n");
    
    if (memcmp(key_a, key_b, sizeof(key_a))) {
        printf("Kyber768 epic fail!\n");
        return 1;
    }
    
    printf("Kyber768 epic success!\n");
    
    return 0;
}
