#include "unity.h"
#include "race_runner/racing_tyre.h"
#include <math.h>

#define GRIP_LEVEL_INCLINATION -0.1

void setUp(void) {
    // This is run before EACH test
}

void tearDown(void) {
    // This is run after EACH test
}

float_t calculate_expected_wear(float_t wear_level, float_t critical_wear_level) {
    return 100.0 / (1 + exp(GRIP_LEVEL_INCLINATION * (wear_level - critical_wear_level)));

}

void test_update_tyre_wear_should_update_wear_level_correctly(void) {
    RacingTyre tyre = {
        .wear_level = 100.0,
        .grip_level = 100.0,
        .critical_wear_level = 50.0
    };
    float_t wear_reduction = 10.0;
    float_t expected_grip_level = calculate_expected_wear(90, tyre.critical_wear_level);

    update_tyre_wear(&tyre, wear_reduction);

    TEST_ASSERT_EQUAL_FLOAT(90.0, tyre.wear_level);
    TEST_ASSERT_EQUAL_FLOAT(expected_grip_level, tyre.grip_level);
}

void test_update_tyre_wear_should_update_grip_level_correctly(void) {
    RacingTyre tyre = {
        .wear_level = 100.0,
        .grip_level = 100.0,
        .critical_wear_level = 50.0
    };
    float_t wear_reduction = 20.0;
    float_t expected_grip_level = calculate_expected_wear(80, tyre.critical_wear_level);

    update_tyre_wear(&tyre, wear_reduction);
    
    TEST_ASSERT_EQUAL_FLOAT(80.0, tyre.wear_level);
    TEST_ASSERT_EQUAL_FLOAT(expected_grip_level, tyre.grip_level);
}

void test_update_tyre_wear_maximum_wear_reduction(void){
    RacingTyre tyre = {
        .wear_level = 50.0,
        .grip_level = 80.0,
        .critical_wear_level = 40.0
    };
    float_t wear_reduction = 50.0;

    update_tyre_wear(&tyre, wear_reduction);

    TEST_ASSERT_EQUAL_FLOAT(0, tyre.wear_level);
    TEST_ASSERT_FLOAT_WITHIN(0.1, 1.8, tyre.grip_level);
}

void test_update_tyre_wear_no_modification_when_wear_reduction_is_zero(void) {
    RacingTyre tyre = {
        .wear_level = 100.0,
        .grip_level = 100.0,
        .critical_wear_level = 50.0
    };
    float_t initial_wear_level = tyre.wear_level;
    float_t initial_grip_level = tyre.grip_level;

    update_tyre_wear(&tyre, 0.0);

    TEST_ASSERT_EQUAL_FLOAT(initial_wear_level, tyre.wear_level);
    TEST_ASSERT_EQUAL_FLOAT(initial_grip_level, tyre.grip_level);
}

void test_update_tyre_wear_no_modification_when_wear_reduction_is_negative(void) {
    RacingTyre tyre = {
        .wear_level = 100.0,
        .grip_level = 100.0,
        .critical_wear_level = 50.0
    };
    float_t initial_wear_level = tyre.wear_level;
    float_t initial_grip_level = tyre.grip_level;

    update_tyre_wear(&tyre, -10);

    TEST_ASSERT_EQUAL_FLOAT(initial_wear_level, tyre.wear_level);
    TEST_ASSERT_EQUAL_FLOAT(initial_grip_level, tyre.grip_level);
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_update_tyre_wear_should_update_wear_level_correctly);
    RUN_TEST(test_update_tyre_wear_should_update_grip_level_correctly);
    RUN_TEST(test_update_tyre_wear_maximum_wear_reduction);
    RUN_TEST(test_update_tyre_wear_no_modification_when_wear_reduction_is_zero);
    return UNITY_END();
}