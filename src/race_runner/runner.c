#include "race_runner/runner.h"
#include "race_runner/racing_tyre.h"
#include <math.h>
#include <stdint.h>
#include <stdio.h>

// TODO: Move this influence factors to the sector struct
#define LOW_SPEED_CORNERS_DOWNFORCE_INFLUENCE 0.5
#define LOW_SPEED_CORNERS_ACCELERATION_INFLUENCE 0.5
#define HIGH_SPEED_CORNERS_DOWNFORCE_INFLUENCE 0.8
#define HIGH_SPEED_CORNERS_ACCELERATION_INFLUENCE 0.6
#define STRAIGHT_DOWNFORCE_INFLUENCE 0.8
#define STRAIGHT_ACCELERATION_INFLUENCE 1.2
#define CAR_FACTOR_DIVISOR 200
// TODO: Move this influence factor to the sector struct
#define TYRE_WEAR_FACTOR 1

float_t calculate_car_factor(Car *car, float_t downforce_influence,
                             float_t acceleration_influence);

float_t calculate_tires_factor(RacingTyre *tyre);

float_t calculate_sector_speed(RacingCar *car, Sector *sector) {
  float_t runner_speed = 0;
  Car *car_info = car->car_info;

  switch (sector->type) {
  case LOW_SPEED_CORNERS:
    runner_speed =
        car_info->max_speed *
        calculate_car_factor(car_info, LOW_SPEED_CORNERS_DOWNFORCE_INFLUENCE,
                             LOW_SPEED_CORNERS_ACCELERATION_INFLUENCE) *
        calculate_tires_factor(car->tyre);
    update_tyre_wear(car->tyre, TYRE_WEAR_FACTOR);
    break;
  case HIGH_SPEED_CORNERS:
    runner_speed =
        car_info->max_speed *
        calculate_car_factor(car_info, HIGH_SPEED_CORNERS_DOWNFORCE_INFLUENCE,
                             HIGH_SPEED_CORNERS_ACCELERATION_INFLUENCE) *
        calculate_tires_factor(car->tyre);
    update_tyre_wear(car->tyre, TYRE_WEAR_FACTOR);
    break;
  case STRAIGHT:
    runner_speed = car_info->max_speed *
                   calculate_car_factor(car_info, STRAIGHT_DOWNFORCE_INFLUENCE,
                                        STRAIGHT_ACCELERATION_INFLUENCE) *
                   calculate_tires_factor(car->tyre);
    update_tyre_wear(car->tyre, TYRE_WEAR_FACTOR);
    break;
  default:
    break;
  }

  return runner_speed;
}

float_t calculate_car_factor(Car *car, float_t downforce_influence,
                             float_t acceleration_influence) {
  return (car->downforce * downforce_influence +
          car->acceleration * acceleration_influence) /
         CAR_FACTOR_DIVISOR;
}

float_t calculate_tires_factor(RacingTyre *tyre) {
  return tyre->grip_level / 100;
}

float_t calculate_lap_duration(RacingCar *car, Circuit *circuit, bool pitstop) {
  float_t total_duration = 0;

  if (car == NULL || circuit == NULL || circuit->psectors == NULL) {
    printf("Error: Invalid input to calculate_lap_duration\n");
    return -1.0; // Return a negative value to indicate an error
  }

  for (uint8_t sector_index = 0; sector_index < circuit->num_sectors;
       ++sector_index) {
    Sector *current_sector = &(circuit->psectors[sector_index]);
    if (current_sector == NULL) {
      printf("Error: Invalid sector at index %d\n", sector_index);
      continue; // Skip this sector and continue with the next one
    }

    float_t speed = calculate_sector_speed(car, current_sector);
    float_t duration = current_sector->length_in_meters / speed;
    if (pitstop && sector_index == 0) {
      duration +=
          circuit->pitstop_duration; // Add the pitstop time to the first sector
      car->tyre->wear_level =
          100; // TODO: Implement pitstop properly actually changing the tyre
    }
    total_duration += duration;
  }

  return total_duration;
}