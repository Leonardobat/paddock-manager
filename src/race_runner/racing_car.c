#include <stdlib.h>
#include "race_runner/racing_car.h"

RacingCar* create_racing_car(Car* car_info, Driver* driver, RacingTyre* tyre) {
    if (car_info == NULL || driver == NULL || tyre == NULL) {
        return NULL;
    }

    RacingCar* new_racing_car = (RacingCar*)malloc(sizeof(RacingCar));
    if (new_racing_car == NULL) {
        return NULL;
    }

    new_racing_car->car_info = car_info;
    new_racing_car->driver = driver;
    new_racing_car->tyre = tyre;

    return new_racing_car;
}

void destroy_racing_car(RacingCar* racing_car) {
    if (racing_car != NULL) {
        // Note: We're not freeing car_info, driver, or tyre here
        // as we don't own these pointers
        free(racing_car);
    }
}