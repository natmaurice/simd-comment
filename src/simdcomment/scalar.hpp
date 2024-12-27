#ifndef SIMDCOMMENT_SCALAR_HPP_
#define SIMDCOMMENT_SCALAR_HPP_

#include <cstdint>
#include <cstddef>

// Note: Requires VBMI2
void simdc_remove_comments(const char* input, size_t len, char* output, size_t& olen);

#endif // SIMDCOMMENT_SCALAR_HPP_
