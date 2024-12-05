#include "unity.h"
#include <stdio.h>
#include <stdlib.h>
#include "race_runner/racing_car.h"

void setUp(void) {
    // This is run before EACH test
}

void tearDown(void) {
    // This is run after EACH test
}

void test_create_racing_car_should_return_null_when_tyre_is_null(void) {
    Car* car = malloc(sizeof(Car));
    Driver* driver = malloc(sizeof(Driver));

    RacingCar* result = create_racing_car(car, driver, NULL);

    TEST_ASSERT_NULL(result);

    free(car);
    free(driver);
}

void test_create_racing_car_returns_null_when_car_info_is_null(void) {
    Driver* driver = malloc(sizeof(Driver));
    RacingTyre* tyre = malloc(sizeof(RacingTyre));

    RacingCar* result = create_racing_car(NULL, driver, tyre);

    TEST_ASSERT_NULL(result);

    free(driver);
    free(tyre);
}

void test_create_racing_car_should_return_null_when_driver_is_null(void) {
    Car* car = malloc(sizeof(Car));
    RacingTyre* tyre = malloc(sizeof(RacingTyre));

    RacingCar* result = create_racing_car(car, NULL, tyre);

    TEST_ASSERT_NULL(result);
    free(car);
    free(tyre);
}

void test_create_racing_car_should_return_null_when_memory_allocation_fails(void) {
    TEST_IGNORE_MESSAGE("Ignores this test until i find a way to use CMock without the Ruby");
    Car mock_car;
    Driver mock_driver;
    RacingTyre mock_tyre;
    RacingCar* result;

    result = create_racing_car(&mock_car, &mock_driver, &mock_tyre);

    TEST_ASSERT_NULL(result);
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_create_racing_car_should_return_null_when_tyre_is_null);
    RUN_TEST(test_create_racing_car_returns_null_when_car_info_is_null);
    RUN_TEST(test_create_racing_car_should_return_null_when_driver_is_null);
    RUN_TEST(test_create_racing_car_should_return_null_when_memory_allocation_fails);
    return UNITY_END();
}
