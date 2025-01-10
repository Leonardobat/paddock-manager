#include "circuit/sector.h"
#include "unity.h"
#include <stdio.h>

void setUp(void) {};
void tearDown(void) {};

void test_is_valid_sector_with_valid_input(void) {
  Sector valid_sector = {
      .number = 1, .type = LOW_SPEED_CORNERS, .length_in_meters = 500};

  TEST_ASSERT_TRUE(is_valid_sector(&valid_sector));
}

void test_is_valid_sector_with_null_input(void) {
  Sector *sector = NULL;

  bool result = is_valid_sector(sector);

  TEST_ASSERT_FALSE(result);
}

void test_is_valid_sector_with_invalid_number(void) {
  Sector sector = {
      .number = 0, .type = LOW_SPEED_CORNERS, .length_in_meters = 500};

  TEST_ASSERT_FALSE(is_valid_sector(&sector));
}

void test_is_valid_sector_with_invalid_length(void) {
  Sector sector = {
      .number = 1,
      .type = STRAIGHT,
      .length_in_meters = 99 // Invalid length (less than MIN_SECTOR_LENGHT)
  };

  TEST_ASSERT_FALSE(is_valid_sector(&sector));
}

void test_is_valid_sector_with_all_invalid_properties(void) {
  Sector invalid_sector = {.number = 0,
                           .type = STRAIGHT + 1, // Invalid type
                           .length_in_meters = 0};

  TEST_ASSERT_FALSE(is_valid_sector(&invalid_sector));
}

int main(void) {
  UNITY_BEGIN();
  RUN_TEST(test_is_valid_sector_with_valid_input);
  RUN_TEST(test_is_valid_sector_with_null_input);
  RUN_TEST(test_is_valid_sector_with_invalid_number);
  RUN_TEST(test_is_valid_sector_with_invalid_length);
  RUN_TEST(test_is_valid_sector_with_all_invalid_properties);
  return UNITY_END();
}
