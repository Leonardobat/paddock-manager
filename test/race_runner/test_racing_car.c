#include "race_runner/racing_car.h"
#include "unity.h"
#include "unity_memory.h"
#include <stdio.h>
#include <stdlib.h>

void setUp(void) { UnityMalloc_StartTest(); }

void tearDown(void) { UnityMalloc_EndTest(); }

void test_create_racing_car_should_return_null_when_tyre_is_null(void) {
  Car *car = malloc(sizeof(Car));
  Driver *driver = malloc(sizeof(Driver));

  RacingCar *result = create_racing_car(car, driver, NULL);

  TEST_ASSERT_NULL(result);

  free(car);
  free(driver);
}

void test_create_racing_car_returns_null_when_car_info_is_null(void) {
  Driver *driver = malloc(sizeof(Driver));
  RacingTyre *tyre = malloc(sizeof(RacingTyre));

  RacingCar *result = create_racing_car(NULL, driver, tyre);

  TEST_ASSERT_NULL(result);

  free(driver);
  free(tyre);
}

void test_create_racing_car_should_return_null_when_driver_is_null(void) {
  Car *car = malloc(sizeof(Car));
  RacingTyre *tyre = malloc(sizeof(RacingTyre));

  RacingCar *result = create_racing_car(car, NULL, tyre);

  TEST_ASSERT_NULL(result);
  free(car);
  free(tyre);
}

void test_create_racing_car_should_return_null_when_memory_allocation_fails(
    void) {
  UnityMalloc_MakeMallocFailAfterCount(3);

  Car *car = malloc(sizeof(Car));
  Driver *driver = malloc(sizeof(Driver));
  RacingTyre *tyre = malloc(sizeof(RacingTyre));
  RacingCar *result = create_racing_car(car, driver, tyre);
  TEST_ASSERT_NULL(result);
  free(car);
  free(driver);
  free(tyre);
}

int main(void) {
  UNITY_BEGIN();
  RUN_TEST(test_create_racing_car_should_return_null_when_tyre_is_null);
  RUN_TEST(test_create_racing_car_returns_null_when_car_info_is_null);
  RUN_TEST(test_create_racing_car_should_return_null_when_driver_is_null);
  RUN_TEST(
      test_create_racing_car_should_return_null_when_memory_allocation_fails);
  return UNITY_END();
}
