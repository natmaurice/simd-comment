#include "scalar.hpp"

void simdc_remove_comments(const char* input, size_t len, char* output, size_t& olen) {

    olen = 0;
    bool commented = false;
    bool quoted = false;
    
    for (size_t i = 0; i < len; i++) {
        char c = input[i];
        
        switch (c) {
        case '#':
            commented = !quoted; // enable if outside quotation marks
            break;
        case '"':
            quoted = (!quoted) & (!commented); // don't quote if inside comment
            break;
        case '\n':
            commented = false;
            break;
        default:
            break;
        }

        output[olen] = c; // branch-free, if commented then will be overwritten in next iteration
        if (!commented) {
            olen++;
        }

    }
}
