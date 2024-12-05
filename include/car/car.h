#ifndef CAR_H
#define CAR_H

#include <math.h>
#include <stdint.h>

typedef struct {
    char scuderia[50];
    float_t max_speed;
    uint8_t acceleration;
    uint8_t downforce;
} Car;

#endif // CAR_H