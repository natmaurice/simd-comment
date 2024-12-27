#include "scalar.hpp"

void simdc_remove_comments(const char* input, size_t len, char* output, size_t& olen) {

    olen = 0;
    bool commented = false;
    
    for (size_t i = 0; i < len; i++) {
        char c = input[i];
        
        switch (c) {
        case '#':
            commented = true;
            break;
        case '\n':
            commented = false;
        default:
            if (!commented) {
                output[olen] = c;
                olen++;
            }
            break;
        }        
    }
}
