#ifndef TVSEG_PARAMS_H
#define TVSEG_PARAMS_H


#include "tvseg/settings/defineparams.h"

namespace  tvseg{
    struct Dim2 {
        unsigned int height;
        unsigned int width;
        Dim2() {
            height = 0;
            width = 0;
        }
    };

    struct Dim3 {
        unsigned int height;
        unsigned int width;
        unsigned int labels;
        Dim3() {
            labels = 0;
            width = 0;
            height = 0;
        }
    };
}

#endif // TVSEG_PARAMS_H
