#include "circuit/circuit.h"
#include "unity.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void setUp(void) {
  // This is run before EACH test
}

void tearDown(void) {
  // This is run after EACH test
}

void test_create_circuit_should_truncate_long_name(void) {
  const char *long_name =
      "This is a very long circuit name that exceeds the maximum length";
  Sector* sectors = malloc(3 * sizeof(Sector));
  uint8_t num_sectors = 1;
  uint8_t total_laps = 1;
  uint8_t pitstop_duration = 1;

  Circuit *result = create_circuit(long_name, sectors, num_sectors, total_laps,
                                   pitstop_duration);

  TEST_ASSERT_NOT_NULL(result);
  TEST_ASSERT_EQUAL_STRING_LEN(long_name, result->name,
                               sizeof(result->name) - 1);
  TEST_ASSERT_EQUAL_UINT8(0, result->name[sizeof(result->name) - 1]);

  destroy_circuit(result);
}

void test_create_circuit_with_valid_inputs(void) {
  const char *name = "Test Circuit";
  Sector *sectors = malloc(3 * sizeof(Sector)); //dummy sector pointer
  uint8_t num_sectors = 3;
  uint8_t total_laps = 1;
  uint8_t pitstop_duration = 1;

  Circuit *result =
      create_circuit(name, sectors, num_sectors, total_laps, pitstop_duration);

  TEST_ASSERT_NOT_NULL(result);
  TEST_ASSERT_EQUAL_STRING(name, result->name);
  TEST_ASSERT_EQUAL_PTR(sectors, result->psectors);
  TEST_ASSERT_EQUAL_UINT8(num_sectors, result->num_sectors);

  destroy_circuit(result);
}

void test_create_circuit_returns_null_when_sectors_is_null(void) {
  const char *name = "Test Circuit";
  Sector *sectors = NULL;
  uint8_t num_sectors = 3;
  uint8_t total_laps = 1;
  uint8_t pitstop_duration = 1;

  Circuit *result =
      create_circuit(name, sectors, num_sectors, total_laps, pitstop_duration);

  TEST_ASSERT_NULL(result);
}

void test_create_circuit_should_return_null_when_num_sectors_is_zero(void) {
  const char *name = "Test Circuit";
  Sector sectors[1] = {{0}}; // Dummy sector
  uint8_t num_sectors = 0;
  uint8_t total_laps = 1;
  uint8_t pitstop_duration = 1;

  Circuit *result =
      create_circuit(name, sectors, num_sectors, total_laps, pitstop_duration);

  TEST_ASSERT_NULL(result);
}

int main(void) {
  UNITY_BEGIN();
  RUN_TEST(test_create_circuit_should_truncate_long_name);
  RUN_TEST(test_create_circuit_with_valid_inputs);
  RUN_TEST(test_create_circuit_returns_null_when_sectors_is_null);
  RUN_TEST(test_create_circuit_should_return_null_when_num_sectors_is_zero);
  return UNITY_END();
}
