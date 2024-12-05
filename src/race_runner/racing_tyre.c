
#include "race_runner/racing_tyre.h"
#include <stdio.h>
#include <math.h>

#define GRIP_LEVEL_INCLINATION -0.1

void update_tyre_wear(RacingTyre *tyre, float_t wear_reduction) {
    if (wear_reduction <= 0.0) {
        return;
    }
    
    //TODO: Improve error handling for invalid inputs
    if (tyre == NULL) {
        printf("Error: Invalid tyre to update wear\n");
        return;
    }
    if (wear_reduction > tyre->wear_level) {
        printf("Error: Wear reduction exceeds current wear level\n");
        return;
    }

    tyre->wear_level -= wear_reduction;

    // Update grip level based on wear reduction, i'm using a sigmoid function (the curve is described on the docs)
    tyre->grip_level = 100.0 / (1 + exp(GRIP_LEVEL_INCLINATION *(tyre->wear_level - tyre->critical_wear_level)));
}