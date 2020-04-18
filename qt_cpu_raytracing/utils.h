#ifndef UTILS_H
#define UTILS_H

#include <stdlib.h>

float floatRand(){
    return static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
}

#endif // UTILS_H
