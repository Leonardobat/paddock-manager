#include "car/car.h"
#include "database/database.h"
#include "unity.h"
#include <stdio.h>
#include <stdlib.h>

sqlite3 *in_memory_db;

void setUp(void) {
  connect_to_database(":memory:", &in_memory_db);
  migrate_tables(in_memory_db);
};

void tearDown(void) { close_connection(in_memory_db); };

void test_save_cars_to_database_for_multiple_scuderias(void) {
  Car cars_to_save[] = {{1, "Ferrari", 340.0, 95, 85},
                        {2, "Ferrari", 338.0, 93, 87},
                        {3, "McLaren", 339.0, 94, 86},
                        {4, "McLaren", 337.0, 92, 88},
                        {5, "RedBull", 341.0, 96, 84}};
  int cars_count = sizeof(cars_to_save) / sizeof(Car);

  int save_result =
      save_cars_to_database(cars_to_save, cars_count, in_memory_db);
  TEST_ASSERT_EQUAL_INT(cars_count, save_result);

  // Verify the cars were saved correctly using a plain SQL query
  sqlite3_stmt *stmt;
  const char *sql = "SELECT id, scuderia, max_speed, acceleration, downforce "
                    "FROM cars ORDER BY id";
  int rc = sqlite3_prepare_v2(in_memory_db, sql, -1, &stmt, NULL);
  TEST_ASSERT_EQUAL_INT(SQLITE_OK, rc);

  int index = 0;
  while (sqlite3_step(stmt) == SQLITE_ROW) {
    TEST_ASSERT_LESS_THAN(
        cars_count,
        index); // Ensure we don't exceed the number of cars we expect

    TEST_ASSERT_EQUAL_INT(cars_to_save[index].id, sqlite3_column_int(stmt, 0));
    TEST_ASSERT_EQUAL_STRING(cars_to_save[index].scuderia,
                             sqlite3_column_text(stmt, 1));
    TEST_ASSERT_EQUAL_FLOAT(cars_to_save[index].max_speed,
                            sqlite3_column_double(stmt, 2));
    TEST_ASSERT_EQUAL_INT(cars_to_save[index].acceleration,
                          sqlite3_column_int(stmt, 3));
    TEST_ASSERT_EQUAL_INT(cars_to_save[index].downforce,
                          sqlite3_column_int(stmt, 4));

    index++;
  }

  TEST_ASSERT_EQUAL_INT(cars_count, index); // Ensure we've checked all cars
  sqlite3_finalize(stmt);
}

void test_load_scuderia_cars_from_database_for_multiple_scuderias(void) {
  // Insert test data directly into the database
  const char *insert_sql = "INSERT INTO cars (id, scuderia, max_speed, "
                           "acceleration, downforce) VALUES "
                           "(1, 'Ferrari', 340.0, 95, 85), "
                           "(2, 'Ferrari', 338.0, 93, 87), "
                           "(3, 'McLaren', 339.0, 94, 86), "
                           "(4, 'McLaren', 337.0, 92, 88), "
                           "(5, 'RedBull', 341.0, 96, 84)";

  int rc = sqlite3_exec(in_memory_db, insert_sql, NULL, NULL, NULL);
  TEST_ASSERT_EQUAL_INT(SQLITE_OK, rc);

  char *scuderias[] = {"Ferrari", "McLaren", "RedBull"};
  int scuderia_count = 3;
  Car *loaded_cars = NULL;

  int result = load_scuderia_cars_from_database(scuderias, scuderia_count,
                                                &loaded_cars, in_memory_db);

  TEST_ASSERT_EQUAL_INT(5, result);
  TEST_ASSERT_NOT_NULL(loaded_cars);

  // Verify the loaded cars
  Car expected_cars[] = {{1, "Ferrari", 340.0, 95, 85},
                         {2, "Ferrari", 338.0, 93, 87},
                         {3, "McLaren", 339.0, 94, 86},
                         {4, "McLaren", 337.0, 92, 88},
                         {5, "RedBull", 341.0, 96, 84}};

  for (int i = 0; i < 5; i++) {
    TEST_ASSERT_EQUAL_INT(expected_cars[i].id, loaded_cars[i].id);
    TEST_ASSERT_EQUAL_STRING(expected_cars[i].scuderia,
                             loaded_cars[i].scuderia);
    TEST_ASSERT_EQUAL_FLOAT(expected_cars[i].max_speed,
                            loaded_cars[i].max_speed);
    TEST_ASSERT_EQUAL_INT(expected_cars[i].acceleration,
                          loaded_cars[i].acceleration);
    TEST_ASSERT_EQUAL_INT(expected_cars[i].downforce, loaded_cars[i].downforce);
  }

  free(loaded_cars);
}

void test_load_scuderia_cars_from_database_no_cars_found(void) {
  char *scuderias[] = {"NonExistentScuderia1", "NonExistentScuderia2"};
  int scuderia_count = 2;
  Car *cars = NULL;
  sqlite3_stmt *stmt = NULL;

  int result = load_scuderia_cars_from_database(scuderias, scuderia_count,
                                                &cars, in_memory_db);

  TEST_ASSERT_EQUAL_INT(0, result);
  TEST_ASSERT_NULL(cars);
}

void test_load_scuderia_cars_from_database_zero_scuderias(void) {
  char **scuderias = NULL;
  int scuderia_count = 0;
  Car *cars = NULL;

  int result = load_scuderia_cars_from_database(scuderias, scuderia_count,
                                                &cars, in_memory_db);

  TEST_ASSERT_EQUAL_INT(0, result);
  TEST_ASSERT_NULL(cars);
}

void test_load_scuderia_cars_from_database_invalid_connection(void) {
  char *scuderias[] = {"Giordano", "Kiwi"};
  int scuderia_count = 2;
  Car *cars = NULL;
  sqlite3 *db = NULL;

  int result =
      load_scuderia_cars_from_database(scuderias, scuderia_count, &cars, db);

  TEST_ASSERT_EQUAL_INT(-1, result);
  TEST_ASSERT_NULL(cars);
}

void test_save_cars_to_database_empty_array(void) {
  Car *cars = NULL;
  int car_count = 0;

  int result = save_cars_to_database(cars, car_count, in_memory_db);

  TEST_ASSERT_EQUAL_INT(0, result);

  // Verify that no cars were added to the database
  sqlite3_stmt *stmt;
  const char *sql = "SELECT COUNT(*) FROM cars";
  int rc = sqlite3_prepare_v2(in_memory_db, sql, -1, &stmt, NULL);
  TEST_ASSERT_EQUAL_INT(SQLITE_OK, rc);

  rc = sqlite3_step(stmt);
  TEST_ASSERT_EQUAL_INT(SQLITE_ROW, rc);

  int count = sqlite3_column_int(stmt, 0);
  TEST_ASSERT_EQUAL_INT(0, count);

  sqlite3_finalize(stmt);
}

int main(void) {
  UNITY_BEGIN();
  RUN_TEST(test_save_cars_to_database_for_multiple_scuderias);
  RUN_TEST(test_load_scuderia_cars_from_database_for_multiple_scuderias);
  RUN_TEST(test_load_scuderia_cars_from_database_no_cars_found);
  RUN_TEST(test_load_scuderia_cars_from_database_zero_scuderias);
  RUN_TEST(test_load_scuderia_cars_from_database_invalid_connection);
  return UNITY_END();
}
