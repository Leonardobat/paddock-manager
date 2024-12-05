#include "car/car.h"
#include "circuit/circuit.h"
#include "race_runner/racing_tyre.h"
#include "race_runner/runner.h"
#include <stdint.h>
#include <stdio.h>

int main() {
  Car myCar = {.scuderia = "Scuderia Giordano",
               .max_speed = 95,
               .downforce = 100,
               .acceleration = 100};
  RacingTyre tyre = {
    .name = "Standard Racing Tyre",
    .critical_wear_level = 40,
    .wear_level = 100,
    .grip_level = 100,
  };

  RacingCar myRacingCar = {.car_info = &myCar, .tyre =  &tyre };

  Sector sectors[] = {
      {1, LOW_SPEED_CORNERS, 1500},
      {2, HIGH_SPEED_CORNERS, 1500},
      {3, STRAIGHT, 1500},
  };
  uint8_t num_sectors = sizeof(sectors) / sizeof(sectors[0]);

  Circuit *myCircuit = create_circuit("Circuit 1", sectors, num_sectors);
  if (myCircuit == NULL) {
    printf("Failed to create circuit\n");
    return 1;
  }

  for (int i = 0; i < 30; ++i) {
    float_t lap_duration = calculate_lap_duration(&myRacingCar, myCircuit);
    printf("[%s] Lap [%d] duration for %s: %f seconds\n", myCircuit->name, i + 1,
           myCar.scuderia, lap_duration);
  }

  destroy_circuit(myCircuit);
  return 0;
}
