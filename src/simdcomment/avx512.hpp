#ifndef SIMDCOMMENT_AVX512_HPP_
#define SIMDCOMMENT_AVX512_HPP_

#include <cstdint>
#include <cstddef>



void simdc_remove_comments_avx512_vbmi2(const char* input, size_t len, char* output, size_t& olen);
void simdc_remove_comments_avx512_vbmi2_v2(const char* input, size_t len, char* output, size_t& olen);

#endif // SIMDCOMMENT_AVX512_HPP_
