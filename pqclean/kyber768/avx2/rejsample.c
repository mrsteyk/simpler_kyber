#include "align.h"
#include "consts.h"
#include "params.h"
#include "rejsample.h"
#include <immintrin.h>
#include <stdint.h>
#include <string.h>

//#define BMI

#define _mm256_cmpge_epu16(a, b) _mm256_cmpeq_epi16(_mm256_max_epu16(a, b), a)
#define _mm_cmpge_epu16(a, b) _mm_cmpeq_epi16(_mm_max_epu16(a, b), a)

unsigned int PQCLEAN_KYBER768_AVX2_rej_uniform_avx(int16_t *restrict r, const uint8_t *buf) {
    unsigned int ctr, pos;
    uint16_t val0, val1;
    uint32_t good;
    uint64_t idx0, idx1, idx2, idx3;
    const __m256i bound  = _mm256_load_si256(&PQCLEAN_KYBER768_AVX2_qdata.vec[_16XQ / 16]);
    const __m256i ones   = _mm256_set1_epi8(1);
    const __m256i mask  = _mm256_set1_epi16(0xFFF);
    const __m256i idx8  = _mm256_set_epi8(15, 14, 14, 13, 12, 11, 11, 10,
                                          9, 8, 8, 7, 6, 5, 5, 4,
                                          11, 10, 10, 9, 8, 7, 7, 6,
                                          5, 4, 4, 3, 2, 1, 1, 0);
    __m256i f0, f1, g0, g1, g2, g3;
    __m128i f, t, pilo, pihi;

    ctr = pos = 0;
    while (ctr <= KYBER_N - 32 && pos <= REJ_UNIFORM_AVX_BUFLEN - 56) {
        f0 = _mm256_loadu_si256((__m256i *)&buf[pos]);
        f1 = _mm256_loadu_si256((__m256i *)&buf[pos + 24]);
        f0 = _mm256_permute4x64_epi64(f0, 0x94);
        f1 = _mm256_permute4x64_epi64(f1, 0x94);
        f0 = _mm256_shuffle_epi8(f0, idx8);
        f1 = _mm256_shuffle_epi8(f1, idx8);
        g0 = _mm256_srli_epi16(f0, 4);
        g1 = _mm256_srli_epi16(f1, 4);
        f0 = _mm256_blend_epi16(f0, g0, 0xAA);
        f1 = _mm256_blend_epi16(f1, g1, 0xAA);
        f0 = _mm256_and_si256(f0, mask);
        f1 = _mm256_and_si256(f1, mask);
        pos += 48;

        g0 = _mm256_cmpgt_epi16(bound, f0);
        g1 = _mm256_cmpgt_epi16(bound, f1);

        g0 = _mm256_packs_epi16(g0, g1);
        good = _mm256_movemask_epi8(g0);

        idx0 = _pdep_u64(good >>  0, 0x0101010101010101);
        idx1 = _pdep_u64(good >>  8, 0x0101010101010101);
        idx2 = _pdep_u64(good >> 16, 0x0101010101010101);
        idx3 = _pdep_u64(good >> 24, 0x0101010101010101);
        idx0 = (idx0 << 8) - idx0;
        idx0  = _pext_u64(0x0E0C0A0806040200, idx0);
        idx1 = (idx1 << 8) - idx1;
        idx1  = _pext_u64(0x0E0C0A0806040200, idx1);
        idx2 = (idx2 << 8) - idx2;
        idx2  = _pext_u64(0x0E0C0A0806040200, idx2);
        idx3 = (idx3 << 8) - idx3;
        idx3  = _pext_u64(0x0E0C0A0806040200, idx3);

        g0 = _mm256_castsi128_si256(_mm_cvtsi64_si128(idx0));
        g1 = _mm256_castsi128_si256(_mm_cvtsi64_si128(idx1));
        g0 = _mm256_inserti128_si256(g0, _mm_cvtsi64_si128(idx2), 1);
        g1 = _mm256_inserti128_si256(g1, _mm_cvtsi64_si128(idx3), 1);

        g2 = _mm256_add_epi8(g0, ones);
        g3 = _mm256_add_epi8(g1, ones);
        g0 = _mm256_unpacklo_epi8(g0, g2);
        g1 = _mm256_unpacklo_epi8(g1, g3);

        f0 = _mm256_shuffle_epi8(f0, g0);
        f1 = _mm256_shuffle_epi8(f1, g1);

        _mm_storeu_si128((__m128i *)&r[ctr], _mm256_castsi256_si128(f0));
        ctr += _mm_popcnt_u32((good >>  0) & 0xFF);
        _mm_storeu_si128((__m128i *)&r[ctr], _mm256_extracti128_si256(f0, 1));
        ctr += _mm_popcnt_u32((good >> 16) & 0xFF);
        _mm_storeu_si128((__m128i *)&r[ctr], _mm256_castsi256_si128(f1));
        ctr += _mm_popcnt_u32((good >>  8) & 0xFF);
        _mm_storeu_si128((__m128i *)&r[ctr], _mm256_extracti128_si256(f1, 1));
        ctr += _mm_popcnt_u32((good >> 24) & 0xFF);
    }

    while (ctr <= KYBER_N - 8 && pos <= REJ_UNIFORM_AVX_BUFLEN - 16) {
        f = _mm_loadu_si128((__m128i *)&buf[pos]);
        f = _mm_shuffle_epi8(f, _mm256_castsi256_si128(idx8));
        t = _mm_srli_epi16(f, 4);
        f = _mm_blend_epi16(f, t, 0xAA);
        f = _mm_and_si128(f, _mm256_castsi256_si128(mask));
        pos += 12;

        t = _mm_cmpgt_epi16(_mm256_castsi256_si128(bound), f);
        good = _mm_movemask_epi8(t);

        good &= 0x5555;
        idx0 = _pdep_u64(good, 0x1111111111111111);
        idx0 = (idx0 << 8) - idx0;
        idx0 = _pext_u64(0x0E0C0A0806040200, idx0);
        pilo = _mm_cvtsi64_si128(idx0);

        pihi = _mm_add_epi8(pilo, _mm256_castsi256_si128(ones));
        pilo = _mm_unpacklo_epi8(pilo, pihi);
        f = _mm_shuffle_epi8(f, pilo);
        _mm_storeu_si128((__m128i *)&r[ctr], f);
        ctr += _mm_popcnt_u32(good);
    }

    while (ctr < KYBER_N && pos <= REJ_UNIFORM_AVX_BUFLEN - 3) {
        val0 = ((buf[pos + 0] >> 0) | ((uint16_t)buf[pos + 1] << 8)) & 0xFFF;
        val1 = ((buf[pos + 1] >> 4) | ((uint16_t)buf[pos + 2] << 4));
        pos += 3;

        if (val0 < KYBER_Q) {
            r[ctr++] = val0;
        }
        if (val1 < KYBER_Q && ctr < KYBER_N) {
            r[ctr++] = val1;
        }
    }

    return ctr;
}
