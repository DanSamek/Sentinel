#ifndef SENTINEL_SIMD_H
#define SENTINEL_SIMD_H

#if defined(ENABLE_AVX) || defined(ENABLE_AVX_512) || defined(ENABLE_SSE)
    #define USE_SIMD 1
#else
    #define USE_SIMD 0
#endif

#if USE_SIMD
#include <immintrin.h>
#endif

#if defined(ENABLE_AVX_512)
    #define simd_type __m512i
    #define simd_jmp 16
    #define SIMD_ADD _mm512_add_epi32
    #define SIMD_SUB _mm512_sub_epi32
    #define SIMD_SET1 _mm512_set1_epi32
    #define SIMD_MIN  _mm512_min_epi32
    #define SIMD_MAX  _mm512_max_epi32
    #define SIMD_MUL  _mm512_mullo_epi32
    #define SIMD_LOAD _mm512_load_si512
    #define SIMD_STORE _mm512_store_si512
#elif defined(ENABLE_AVX)
    #define simd_type __m256i
    #define simd_jmp 8
    #define SIMD_ADD _mm256_add_epi32
    #define SIMD_SUB _mm256_sub_epi32
    #define SIMD_SET1 _mm256_set1_epi32
    #define SIMD_MIN  _mm256_min_epi32
    #define SIMD_MAX  _mm256_max_epi32
    #define SIMD_MUL  _mm256_mullo_epi32
    #define SIMD_LOAD _mm256_load_si256
    #define SIMD_STORE _mm256_store_si256
#elif defined(ENABLE_SSE)
    #define simd_type __m128i
    #define simd_jmp 4
    #define SIMD_ADD _mm_add_epi32
    #define SIMD_SUB _mm_sub_epi32
    #define SIMD_SET1 _mm_set1_epi32
    #define SIMD_MIN  _mm_min_epi32
    #define SIMD_MAX  _mm_max_epi32
    #define SIMD_MUL  _mm_mullo_epi32
    #define SIMD_LOAD _mm_load_si128
    #define SIMD_STORE _mm_store_si128
#endif



#endif //SENTINEL_SIMD_H
