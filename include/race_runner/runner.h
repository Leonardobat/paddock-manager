#ifndef RUNNER_H
#define RUNNER_H

#include "circuit/circuit.h"
#include "circuit/sector.h"
#include "racing_car.h"
#include <math.h>
#include <stdbool.h>

float_t calculate_sector_speed(RacingCar* car, Sector* sector);

float_t calculate_lap_duration(RacingCar* car, Circuit* circuit, bool Pitstop);

#endif // RUNNER_H