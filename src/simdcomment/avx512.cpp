#include "avx512.hpp"

#include <immintrin.h>

uint64_t prefix_xor_u64(uint64_t v) {

    for (size_t s = 1; s < 64; s *= 2) {
        v ^= (v << s);
    }
    return v;
}

uint64_t segscan_or_u64(uint64_t v, uint64_t mreset) {

    uint64_t v0, v1;
    uint64_t m0;
    
    // Step 1
    v0 = (v << 1) | v;
    v1 = (v0 & (~mreset)) | (v & mreset);

    m0 = (mreset << 1) | mreset;
    
    // Step 2
    v0 = (v1 << 2) | v1;
    v1 = (v0 & (~m0)) | (v1 & m0);
    m0 = (m0 << 2) | m0;

    // Step 3
    v0 = (v1 << 4) | v1;
    v1 = (v0 & (~m0)) | (v1 & m0);
    m0 = (m0 << 4) | m0;

    // Step 4
    v0 = (v1 << 8) | v1;
    v1 = (v0 & (~m0)) | (v1 & m0);
    m0 = (m0 << 8) | m0;

    // Step 5
    v0 = (v1 << 16) | v1;
    v1 = (v0 & (~m0)) | (v1 & m0);
    m0 = (m0 << 16) | m0;

    // Step 6
    v0 = (v1 << 32) | v1;
    v1 = (v0 & (~m0)) | (v1 & m0);

    return v1;
}

void simdc_remove_comments_avx512_vbmi2(const char* input, size_t len, char* output, size_t& olen) {

    constexpr size_t CARD_SIMD = 64; // Read/Write 64 elements per SIMD


    const __m512i vcomment = _mm512_set1_epi8('#');
    const __m512i veol = _mm512_set1_epi8('\n');
    const __m512i vquote = _mm512_set1_epi8('"');
    
    uint64_t mcarry = 0;
    uint64_t mquotecarry = 0;
    olen = 0;
    
    // Main loop
    size_t i = 0;
    for (i = 0; i + CARD_SIMD < len; i += 64) {
        
        __m512i vin = _mm512_loadu_si512(input + i);

        uint64_t mcomment = _mm512_cmpeq_epi8_mask(vin, vcomment);
        uint64_t meol = _mm512_cmpeq_epi8_mask(vin, veol);
        uint64_t mquote = _mm512_cmpeq_epi8_mask(vin, vquote);

        mquote |= mquotecarry;
        mquote = prefix_xor_u64(mquote);
        
        // Perform segmented scan on scalar elements
        // A SIMD implementation is possible, but would have to deal with
        // inter-lane operation and port 5
        mcarry &= (~meol); // mask carry if first bit is eol 
        mcomment &= (~mquote); // don't include comment if quoted
        mcomment |= mcarry;
        
        mcomment = segscan_or_u64(mcomment, meol);
        
        // Compress store
        // Perform compress and store separately due to Zen 4 performance issue
        // On Intel and Zen 5, the following instructions can be merged into a single
        // _mm512_maskz_compress_storeu
        __m512i vuncomment = _mm512_maskz_compress_epi8(~mcomment, vin);
        _mm512_storeu_epi8(output + olen, vuncomment);

        olen += __builtin_popcountl(~mcomment);
        
        // Register rotation
        mcarry = mcomment >> 63;
        mquotecarry = mquote >> 63;
    }    

    // Tail
    __mmask64 mleftover = 0xffffffffffffffff >> (64 - len - i);

    __m512i vin = _mm512_maskz_loadu_epi8(mleftover, input + i);

    uint64_t mcomment = _mm512_cmpeq_epi8_mask(vin, vcomment);
    uint64_t meol = _mm512_cmpeq_epi8_mask(vin, veol);
    uint64_t mquote = _mm512_cmpeq_epi8_mask(vin, vquote);

    mquote |= mquotecarry;
    mquote = prefix_xor_u64(mquote);
    
    // Perform segmented scan on scalar elements
    // A SIMD implementation is possible, but would have to deal with
    // inter-lane operation and port 5
    mcarry &= (~meol); // mask carry if first bit is eol 
    mcomment &= (~mquote);
    mcomment |= mcarry;
    mcomment = segscan_or_u64(mcomment, meol);
    
    // Compress store
    // Perform compress and store separately due to Zen 4 performance issue
    // On Intel and Zen 5, the following instructions can be merged into a single
    // _mm512_maskz_compress_storeu
    __m512i vuncomment = _mm512_maskz_compress_epi8(~mcomment, vin);
    _mm512_mask_storeu_epi8(output + olen, mleftover, vuncomment);

    olen += __builtin_popcountl((~mcomment) & mleftover);
}
