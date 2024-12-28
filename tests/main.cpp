#include <functional>
#include <random>
#include <bit>

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

        char* output = new char[len + 1];
        
        size_t olen;

        fun(input, len, output, olen);
        output[olen] = '\0'; // Used for string comparison
        
        REQUIRE(olen == expected_len);
        REQUIRE(std::string(output) == std::string(expected_output));
        
        delete[] output;
    }
    
}

void test_simdcomment_compare(SIMDCommentFun fun_ref, SIMDCommentFun fun) {

    size_t tries = 1000;

    constexpr size_t LEN = 1000;

    std::uniform_int_distribution<> distr(100); // 1% chance of eol & 1% chance of '#'
    
    constexpr uint8_t KEY_COMMENT = 0;
    constexpr uint8_t KEY_NEWLINE = 1;

    char input[LEN + 1];
    char output_ref[LEN + 1];
    char output_res[LEN + 1];

    input[LEN] = '\0';
    
    for (size_t i = 0; i < tries; i++) {
        std::mt19937 mt(i);

        for (size_t j = 0; j < LEN; j++) {
            uint8_t val = distr(mt);
            uint8_t c = 'a';
            if (val == KEY_COMMENT) { // 
                c = '#';
            } else if (val == KEY_NEWLINE) {
                c = '\n';
            }
            input[i] = c;
        }
        
        size_t len_ref;
        size_t len_res;
        
        fun_ref(input, LEN, output_ref, len_ref);
        fun(input, LEN, output_res, len_res);

        output_ref[len_ref] = '\0';
        output_res[len_res] = '\0';
        
        REQUIRE(len_res == len_ref);
        REQUIRE(std::string(output_res) == std::string(output_ref));
    }
}

#ifdef SIMDCOMMENT_ENABLE_AVX512
extern uint64_t segscan_or_u64(uint64_t v, uint64_t mreset);
extern uint64_t segscan_or_u64_v2(uint64_t v, uint64_t mreset);

TEST_CASE("segscan_or_u64_v2") {

    SECTION("v=0, mreset = 0") {
        
        uint64_t res = segscan_or_u64_v2(0, 0);
        REQUIRE(res == 0);
    }
    SECTION("v = 1b, mreset = 0") {

        uint64_t res = segscan_or_u64_v2(1, 0);
        REQUIRE(res == std::bit_cast<uint64_t>(-1LL));
    }
    SECTION("v = 1b, mreset = 1b") {
        
        uint64_t res = segscan_or_u64_v2(1, 1);
        REQUIRE(res == std::bit_cast<uint64_t>(-1LL));
    }
    SECTION("v = 1b, mreset = 10b") {
        
        uint64_t res = segscan_or_u64_v2(0b1, 0b01);
        REQUIRE(res == 0b01);
    }
    SECTION("v = 1001b, mreset = 0010b") {
        
        uint64_t res = segscan_or_u64_v2(0b1001, 0b10);
        REQUIRE(res == 0b1111111111111111111111111111111111111111111111111111111111111001);
    }
    SECTION("v = 1111b, mreset = 0b100000") {
        
        uint64_t res = segscan_or_u64_v2(0b1111, 0b100000);
        REQUIRE(res == 0b11111);
    }
    
    SECTION("Random") {

        constexpr size_t tries = 1000;
    
        for (size_t i = 0; i < tries; i++) {
            std::mt19937_64 mt(i);

            uint64_t v = mt();
            uint64_t mreset = mt();

            uint64_t out_ref = segscan_or_u64(v, mreset);
            uint64_t out_res = segscan_or_u64_v2(v, mreset);

            REQUIRE(out_res == out_ref);
        }
    }
}
#endif // SIMDCOMMENT_ENABLE_AVX512


TEST_CASE("SIMD Comment - Scalar") {
    test_simdcomment_fn(simdc_remove_comments);
}

#ifdef SIMDCOMMENT_ENABLE_AVX512
TEST_CASE("SIMD Comment - AVX512") {
    test_simdcomment_fn(simdc_remove_comments_avx512_vbmi2);

    test_simdcomment_compare(simdc_remove_comments, simdc_remove_comments_avx512_vbmi2);
}
#endif // SIMDCOMMENT_ENABLE_AVX512


