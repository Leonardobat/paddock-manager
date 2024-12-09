#include "race_runner/runner.h"
#include "unity.h"
#include <math.h>

void setUp(void) {
  // This is run before EACH test
}

void tearDown(void) {
  // This is run after EACH test
}

void test_full_race_with_only_one_car(void) {
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

  RacingCar myRacingCar = {.car_info = &myCar, .tyre = &tyre};

  Sector sectors[] = {
      {1, LOW_SPEED_CORNERS, 1500},
      {2, HIGH_SPEED_CORNERS, 1500},
      {3, STRAIGHT, 1500},
  };
  uint8_t num_sectors = sizeof(sectors) / sizeof(sectors[0]);

  Circuit *myCircuit =
      create_circuit("Circuit 1", sectors, num_sectors, 30, 20);

  for (int i = 0; i < myCircuit->total_laps; ++i) {
    float_t initial_wear_level = myRacingCar.tyre->wear_level;
    bool pitstop = (i % 10 == 0) && i > 0;
    float_t lap_duration =
        calculate_lap_duration(&myRacingCar, myCircuit, pitstop);
    if (!pitstop) {
      TEST_ASSERT_LESS_THAN_FLOAT(initial_wear_level,
                                     myRacingCar.tyre->wear_level);
      TEST_ASSERT_FLOAT_WITHIN(3, 70, lap_duration);
    } else {
      TEST_ASSERT_GREATER_THAN_FLOAT(initial_wear_level,
                                  myRacingCar.tyre->wear_level);
      TEST_ASSERT_FLOAT_WITHIN(1, 92, lap_duration);
    }
  }
}

int main(void) {
  UNITY_BEGIN();
  RUN_TEST(test_full_race_with_only_one_car);
  return UNITY_END();
}