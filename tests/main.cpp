#include <functional>
#define CATCH_CONFIG_MAIN
#include <catch2/catch_all.hpp>


#include <immintrin.h>

#include <simdcomment/scalar.hpp>

#ifdef SIMDCOMMENT_ENABLE_AVX512
#include <simdcomment/avx512.hpp>
#endif // SIMDCOMMENT_ENABLE_AVX512



using SIMDCommentFun = std::function<void(const char*, size_t, char*, size_t&)>;

void test_simdcomment_fn(SIMDCommentFun fun) {
    SECTION ("Test w/ predefined input") {

        const char* input =
            "Lorem ipsum dolor sit amet, consectetur adipiscing elit\n"
            ", sed do #eiusmod tempor incididunt ut labore et dolore magna aliqua.\n"
            "Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. \n"
            "Duis"
            "# aute \n"
            "#irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum\n";
        
        const char* expected_output =
            "Lorem ipsum dolor sit amet, consectetur adipiscing elit\n"
            ", sed do \n"
            "Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. \n"
            "Duis"
            "\n"
            "\n";

        size_t len = strlen(input);
        size_t expected_len = strlen(expected_output);

        char* output = new char[expected_len + 1];
        
        size_t olen;

        fun(input, len, output, olen);
        output[olen] = '\0'; // Used for string comparison
        
        REQUIRE(olen == expected_len);
        REQUIRE(std::string(output) == std::string(expected_output));
        
        delete[] output;
    }
    
}

TEST_CASE("Test - SIMD Comment - Scalar") {
    test_simdcomment_fn(simdc_remove_comments);

#ifdef SIMDCOMMENT_ENABLE_AVX512
    test_simdcomment_fn(simdc_remove_comments_avx512_vbmi2);
#endif // SIMDCOMMENT_ENABLE_AVX512
}


