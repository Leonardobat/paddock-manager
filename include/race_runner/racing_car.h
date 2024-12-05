#ifndef RACING_CAR_H
#define RACING_CAR_H

#include "car/car.h"
#include "driver/driver.h"
#include "racing_tyre.h"

typedef struct {
    Car* car_info;
    Driver* driver;
    RacingTyre* tyre;
} RacingCar;

RacingCar* create_racing_car(Car* car_info, Driver* driver, RacingTyre* tyre);

void destroy_racing_car(RacingCar* racing_car);

#endif // RACING_CAR_H