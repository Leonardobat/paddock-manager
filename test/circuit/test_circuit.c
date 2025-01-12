#include "circuit/circuit.h"
#include "database/database.h"
#include "unity.h"
#include "unity_internals.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

sqlite3 *in_memory_db;

void setUp(void) {
  connect_to_database(":memory:", &in_memory_db);
  migrate_tables(in_memory_db);
};

void tearDown(void) { close_connection(in_memory_db); };

void test_create_circuit_should_truncate_long_name(void) {
  const char *long_name =
      "This is a very long circuit name that exceeds the maximum length";
  Sector *sectors = malloc(3 * sizeof(Sector));
  TEST_ASSERT_NOT_NULL(sectors);

  sectors[0] =
      (Sector){.number = 1, .type = STRAIGHT, .length_in_meters = 1000};
  sectors[1] =
      (Sector){.number = 2, .type = LOW_SPEED_CORNERS, .length_in_meters = 500};
  sectors[2] = (Sector){
      .number = 3, .type = HIGH_SPEED_CORNERS, .length_in_meters = 800};

  uint8_t num_sectors = 3;
  uint8_t total_laps = 50;
  uint8_t pitstop_duration = 25;

  Circuit *result = create_circuit(long_name, sectors, num_sectors, total_laps,
                                   pitstop_duration);

  TEST_ASSERT_NOT_NULL(result);
  TEST_ASSERT_EQUAL_STRING_LEN(long_name, result->name,
                               sizeof(result->name) - 1);
  TEST_ASSERT_EQUAL_UINT8(0, result->name[sizeof(result->name) - 1]);
  TEST_ASSERT_EQUAL_UINT8(num_sectors, result->num_sectors);
  TEST_ASSERT_EQUAL_UINT8(total_laps, result->total_laps);
  TEST_ASSERT_EQUAL_UINT8(pitstop_duration, result->pitstop_duration);

  // Check if sectors were copied correctly
  for (int i = 0; i < num_sectors; i++) {
    TEST_ASSERT_EQUAL_UINT8(sectors[i].number, result->psectors[i].number);
    TEST_ASSERT_EQUAL_INT(sectors[i].type, result->psectors[i].type);
    TEST_ASSERT_EQUAL_UINT16(sectors[i].length_in_meters,
                             result->psectors[i].length_in_meters);
  }

  destroy_circuit(result);
}

void test_create_circuit_with_valid_inputs(void) {
  const char *name = "Test Circuit";
  Sector *sectors = malloc(3 * sizeof(Sector));
  TEST_ASSERT_NOT_NULL(sectors);

  sectors[0] =
      (Sector){.number = 1, .type = STRAIGHT, .length_in_meters = 1000};
  sectors[1] =
      (Sector){.number = 2, .type = LOW_SPEED_CORNERS, .length_in_meters = 500};
  sectors[2] = (Sector){
      .number = 3, .type = HIGH_SPEED_CORNERS, .length_in_meters = 800};

  uint8_t num_sectors = 3;
  uint8_t total_laps = 1;
  uint8_t pitstop_duration = 1;

  Circuit *result =
      create_circuit(name, sectors, num_sectors, total_laps, pitstop_duration);

  TEST_ASSERT_NOT_NULL(result);
  TEST_ASSERT_EQUAL_STRING(name, result->name);
  TEST_ASSERT_EQUAL_PTR(sectors, result->psectors);
  TEST_ASSERT_EQUAL_UINT8(num_sectors, result->num_sectors);

  for (int i = 0; i < num_sectors; i++) {
    TEST_ASSERT_EQUAL_UINT8(sectors[i].number, result->psectors[i].number);
    TEST_ASSERT_EQUAL_INT(sectors[i].type, result->psectors[i].type);
    TEST_ASSERT_EQUAL_UINT16(sectors[i].length_in_meters,
                             result->psectors[i].length_in_meters);
  }

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

void test_load_circuit_from_db_should_return_negative_one_when_db_is_null(
    void) {
  uint32_t id = 1;
  Circuit *circuit = NULL;
  sqlite3 *db = NULL;

  int result = load_circuit_from_db(id, &circuit, db);

  TEST_ASSERT_EQUAL_INT(-1, result);
  TEST_ASSERT_NULL(circuit);
}

void test_load_circuit_from_db_should_load_all_data_correctly(void) {
  uint32_t id = 1;
  Circuit *circuit = NULL;

  // Mock database setup
  sqlite3_stmt *stmt_mock;
  sqlite3_prepare_v2(
      in_memory_db,
      "INSERT INTO circuit (id, name, sectors, total_laps, pitstop_duration) "
      "VALUES (1, 'Test Circuit', '[{\"number\": 1, \"type\": 2, "
      "\"length_in_meters\": 1000}, {\"number\": 2, \"type\": 1, "
      "\"length_in_meters\": 500}]', 50, 25)",
      -1, &stmt_mock, NULL);
  sqlite3_step(stmt_mock);
  sqlite3_finalize(stmt_mock);

  int result = load_circuit_from_db(id, &circuit, in_memory_db);

  TEST_ASSERT_EQUAL_INT(SQLITE_OK, result);
  TEST_ASSERT_NOT_NULL(circuit);
  TEST_ASSERT_EQUAL_UINT32(1, circuit->id);
  TEST_ASSERT_EQUAL_STRING("Test Circuit", circuit->name);
  TEST_ASSERT_EQUAL_UINT8(2, circuit->num_sectors);
  TEST_ASSERT_EQUAL_UINT8(50, circuit->total_laps);
  TEST_ASSERT_EQUAL_UINT8(25, circuit->pitstop_duration);

  TEST_ASSERT_NOT_NULL(circuit->psectors);
  TEST_ASSERT_EQUAL_UINT8(1, circuit->psectors[0].number);
  TEST_ASSERT_EQUAL_INT(STRAIGHT, circuit->psectors[0].type);
  TEST_ASSERT_EQUAL_UINT16(1000, circuit->psectors[0].length_in_meters);

  TEST_ASSERT_EQUAL_UINT8(2, circuit->psectors[1].number);
  TEST_ASSERT_EQUAL_INT(HIGH_SPEED_CORNERS, circuit->psectors[1].type);
  TEST_ASSERT_EQUAL_UINT16(500, circuit->psectors[1].length_in_meters);

  destroy_circuit(circuit);
}

void test_save_circuit_to_db_with_null_circuit(void) {
  Circuit *circuit = NULL;
  int result = save_circuit_to_db(circuit, in_memory_db);

  TEST_ASSERT_EQUAL_INT(-1, result);
}

void test_save_circuit_to_db_with_null_db(void) {
  Circuit circuit = {}; // Dummy circuit
  int result = save_circuit_to_db(&circuit, NULL);

  TEST_ASSERT_EQUAL_INT(-1, result);
}

void test_save_circuit_to_db_with_valid_input(void) {
  Circuit *circuit = malloc(sizeof(Circuit));
  TEST_ASSERT_NOT_NULL(circuit);

  strcpy(circuit->name, "Test Circuit");
  circuit->id = 1;
  circuit->total_laps = 50;
  circuit->pitstop_duration = 25;
  circuit->num_sectors = 2;

  circuit->psectors = malloc(2 * sizeof(Sector));
  TEST_ASSERT_NOT_NULL(circuit->psectors);

  circuit->psectors[0] =
      (Sector){.number = 1, .type = STRAIGHT, .length_in_meters = 1000};
  circuit->psectors[1] =
      (Sector){.number = 2, .type = LOW_SPEED_CORNERS, .length_in_meters = 500};

  int result = save_circuit_to_db(circuit, in_memory_db);

  TEST_ASSERT_EQUAL_INT(SQLITE_OK, result);

  // Verify the circuit was saved correctly
  sqlite3_stmt *stmt;
  const char *sql = "SELECT id, name, json(sectors) as sectors, total_laps, "
                    "pitstop_duration FROM circuit WHERE id = 1";
  int rc = sqlite3_prepare_v2(in_memory_db, sql, -1, &stmt, NULL);
  TEST_ASSERT_EQUAL_INT(SQLITE_OK, rc);

  rc = sqlite3_step(stmt);
  TEST_ASSERT_EQUAL_INT(SQLITE_ROW, rc);

  TEST_ASSERT_EQUAL_INT(1, sqlite3_column_int(stmt, 0));
  TEST_ASSERT_EQUAL_STRING("Test Circuit", sqlite3_column_text(stmt, 1));
  TEST_ASSERT_EQUAL_STRING(
      "[{\"number\":1,\"type\":2,\"length_in_meters\":1000},{\"number\":2,"
      "\"type\":0,\"length_in_meters\":500}]",
      sqlite3_column_text(stmt, 2));
  TEST_ASSERT_EQUAL_INT(50, sqlite3_column_int(stmt, 3));
  TEST_ASSERT_EQUAL_INT(25, sqlite3_column_int(stmt, 4));

  sqlite3_finalize(stmt);
  free(circuit->psectors);
  free(circuit);
}

int main(void) {
  UNITY_BEGIN();
  RUN_TEST(test_create_circuit_should_truncate_long_name);
  RUN_TEST(test_create_circuit_with_valid_inputs);
  RUN_TEST(test_create_circuit_returns_null_when_sectors_is_null);
  RUN_TEST(test_create_circuit_should_return_null_when_num_sectors_is_zero);
  RUN_TEST(test_load_circuit_from_db_should_load_all_data_correctly);
  RUN_TEST(
      test_load_circuit_from_db_should_return_negative_one_when_db_is_null);
  RUN_TEST(test_save_circuit_to_db_with_valid_input);
  RUN_TEST(test_save_circuit_to_db_with_null_circuit);
  RUN_TEST(test_save_circuit_to_db_with_null_db);
  return UNITY_END();
}
