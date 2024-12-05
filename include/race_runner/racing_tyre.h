#ifndef RACING_TYRES_H
#define RACING_TYRES_H

#include <math.h>
#include <stdint.h>
typedef struct {
    char name[50];
    uint8_t critical_wear_level;
    float_t wear_level;
    float_t grip_level;
} RacingTyre;

void update_tyre_wear(RacingTyre* tyre, float_t wear_reduction);

#endif // RACING_TYRES_H