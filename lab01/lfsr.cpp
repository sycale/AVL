#include <iostream>
#include "lfsr.h"

unsigned get_bit(unsigned r, unsigned n){
    return (r >> n) & 1;
}

void lfsr_calculate(uint16_t *reg) {
        unsigned r = *reg & 1;
        
        r ^= get_bit(*reg, 2);
        r ^= get_bit(*reg, 3);
        r ^= get_bit(*reg, 5);

        *reg = (*reg >> 1) | (r<< 15);
}

