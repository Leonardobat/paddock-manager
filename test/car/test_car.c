// #include "unity.h"
// #include <stdio.h>
// #include "car/car.h"

// void setUp(void);
// void tearDown(void);

// void test_load_scuderia_cars_from_database_multiple_valid_scuderias(void)
// {
//     char *scuderias[] = {"Ferrari", "McLaren", "RedBull"};
//     int scuderia_count = 3;
//     Car *cars = NULL;
//     sqlite3 *db = NULL;
//     sqlite3_stmt *stmt = NULL;
//     int expected_car_count = 5;

//     // Mock the sqlite3_prepare_v2 function for count query
//     sqlite3_prepare_v2_ExpectAndReturn(db, NULL, -1, &stmt, NULL, SQLITE_OK);
//     sqlite3_prepare_v2_IgnoreArg_zSql();
//     sqlite3_prepare_v2_IgnoreArg_ppStmt();

//     // Mock binding parameters
//     for (int i = 0; i < scuderia_count; i++) {
//         sqlite3_bind_text_ExpectAndReturn(stmt, i+1, scuderias[i], -1, SQLITE_STATIC, SQLITE_OK);
//     }

//     // Mock the sqlite3_step and sqlite3_column_int for count query
//     sqlite3_step_ExpectAndReturn(stmt, SQLITE_ROW);
//     sqlite3_column_int_ExpectAndReturn(stmt, 0, expected_car_count);

//     // Mock the sqlite3_finalize
//     sqlite3_finalize_ExpectAndReturn(stmt, SQLITE_OK);

//     // Mock the sqlite3_prepare_v2 function for select query
//     sqlite3_prepare_v2_ExpectAndReturn(db, NULL, -1, &stmt, NULL, SQLITE_OK);
//     sqlite3_prepare_v2_IgnoreArg_zSql();
//     sqlite3_prepare_v2_IgnoreArg_ppStmt();

//     // Mock binding parameters again
//     for (int i = 0; i < scuderia_count; i++) {
//         sqlite3_bind_text_ExpectAndReturn(stmt, i+1, scuderias[i], -1, SQLITE_STATIC, SQLITE_OK);
//     }

//     // Mock the sqlite3_step and sqlite3_column functions for each car
//     for (int i = 0; i < expected_car_count; i++) {
//         sqlite3_step_ExpectAndReturn(stmt, SQLITE_ROW);
//         sqlite3_column_int_ExpectAndReturn(stmt, 0, i+1);
//         sqlite3_column_text_ExpectAndReturn(stmt, 1, (const unsigned char*)"TestScuderia");
//         sqlite3_column_double_ExpectAndReturn(stmt, 2, 300.0);
//         sqlite3_column_int_ExpectAndReturn(stmt, 3, 90);
//         sqlite3_column_int_ExpectAndReturn(stmt, 4, 80);
//     }

//     // Mock the final sqlite3_step to end the loop
//     sqlite3_step_ExpectAndReturn(stmt, SQLITE_DONE);

//     // Mock the final sqlite3_finalize
//     sqlite3_finalize_ExpectAndReturn(stmt, SQLITE_OK);

//     int result = load_scuderia_cars_from_database(scuderias, scuderia_count, &cars, db);

//     TEST_ASSERT_EQUAL_INT(expected_car_count, result);
//     TEST_ASSERT_NOT_NULL(cars);

//     // Clean up
//     free(cars);
// }



// void test_load_scuderia_cars_from_database_no_cars_found(void)
// {
//     char *scuderias[] = {"NonExistentScuderia1", "NonExistentScuderia2"};
//     int scuderia_count = 2;
//     Car *cars = NULL;
//     sqlite3 *db = NULL;
//     sqlite3_stmt *stmt = NULL;

//     // Mock the sqlite3_prepare_v2 function for count query
//     sqlite3_prepare_v2_ExpectAndReturn(db, NULL, -1, &stmt, NULL, SQLITE_OK);
//     sqlite3_prepare_v2_IgnoreArg_zSql();
//     sqlite3_prepare_v2_IgnoreArg_ppStmt();

//     // Mock binding parameters
//     for (int i = 0; i < scuderia_count; i++) {
//         sqlite3_bind_text_ExpectAndReturn(stmt, i+1, scuderias[i], -1, SQLITE_STATIC, SQLITE_OK);
//     }

//     // Mock the sqlite3_step and sqlite3_column_int for count query (return 0 cars)
//     sqlite3_step_ExpectAndReturn(stmt, SQLITE_ROW);
//     sqlite3_column_int_ExpectAndReturn(stmt, 0, 0);

//     // Mock the sqlite3_finalize
//     sqlite3_finalize_ExpectAndReturn(stmt, SQLITE_OK);

//     int result = load_scuderia_cars_from_database(scuderias, scuderia_count, &cars, db);

//     TEST_ASSERT_EQUAL_INT(0, result);
//     TEST_ASSERT_NULL(cars);
// }

// void test_load_scuderia_cars_from_database_single_scuderia(void)
// {
//     char *scuderias[] = {"Ferrari"};
//     int scuderia_count = 1;
//     Car *cars = NULL;
//     sqlite3 *db = NULL;
//     sqlite3_stmt *stmt = NULL;
//     int expected_car_count = 3;

//     sqlite3_prepare_v2_ExpectAndReturn(db, NULL, -1, &stmt, NULL, SQLITE_OK);
//     sqlite3_prepare_v2_IgnoreArg_zSql();
//     sqlite3_prepare_v2_IgnoreArg_ppStmt();

//     sqlite3_bind_text_ExpectAndReturn(stmt, 1, scuderias[0], -1, SQLITE_STATIC, SQLITE_OK);

//     sqlite3_step_ExpectAndReturn(stmt, SQLITE_ROW);
//     sqlite3_column_int_ExpectAndReturn(stmt, 0, expected_car_count);

//     sqlite3_finalize_ExpectAndReturn(stmt, SQLITE_OK);

//     sqlite3_prepare_v2_ExpectAndReturn(db, NULL, -1, &stmt, NULL, SQLITE_OK);
//     sqlite3_prepare_v2_IgnoreArg_zSql();
//     sqlite3_prepare_v2_IgnoreArg_ppStmt();

//     sqlite3_bind_text_ExpectAndReturn(stmt, 1, scuderias[0], -1, SQLITE_STATIC, SQLITE_OK);

//     for (int i = 0; i < expected_car_count; i++) {
//         sqlite3_step_ExpectAndReturn(stmt, SQLITE_ROW);
//         sqlite3_column_int_ExpectAndReturn(stmt, 0, i+1);
//         sqlite3_column_text_ExpectAndReturn(stmt, 1, (const unsigned char*)"Ferrari");
//         sqlite3_column_double_ExpectAndReturn(stmt, 2, 330.0);
//         sqlite3_column_int_ExpectAndReturn(stmt, 3, 95);
//         sqlite3_column_int_ExpectAndReturn(stmt, 4, 85);
//     }

//     sqlite3_step_ExpectAndReturn(stmt, SQLITE_DONE);

//     sqlite3_finalize_ExpectAndReturn(stmt, SQLITE_OK);

//     int result = load_scuderia_cars_from_database(scuderias, scuderia_count, &cars, db);

//     TEST_ASSERT_EQUAL_INT(expected_car_count, result);
//     TEST_ASSERT_NOT_NULL(cars);
//     TEST_ASSERT_EQUAL_STRING("Ferrari", cars[0].scuderia);
//     TEST_ASSERT_EQUAL_DOUBLE(330.0, cars[0].max_speed);
//     TEST_ASSERT_EQUAL_INT(95, cars[0].acceleration);
//     TEST_ASSERT_EQUAL_INT(85, cars[0].downforce);

//     free(cars);
// }

// void test_load_scuderia_cars_from_database_zero_scuderias(void)
// {
//     char **scuderias = NULL;
//     int scuderia_count = 0;
//     Car *cars = NULL;
//     sqlite3 *db = NULL;

//     // Expect no SQLite calls to be made
//     sqlite3_prepare_v2_ExpectAndReturn(db, NULL, -1, NULL, NULL, SQLITE_OK);
//     sqlite3_prepare_v2_IgnoreArg_zSql();
//     sqlite3_prepare_v2_IgnoreArg_ppStmt();

//     sqlite3_step_ExpectAndReturn(NULL, SQLITE_ROW);
//     sqlite3_column_int_ExpectAndReturn(NULL, 0, 0);

//     sqlite3_finalize_ExpectAnyArgs();

//     int result = load_scuderia_cars_from_database(scuderias, scuderia_count, &cars, db);

//     TEST_ASSERT_EQUAL_INT(0, result);
//     TEST_ASSERT_NULL(cars);
// }


// void test_load_scuderia_cars_from_database_invalid_connection(void)
// {
//     char *scuderias[] = {"Ferrari", "McLaren"};
//     int scuderia_count = 2;
//     Car *cars = NULL;
//     sqlite3 *db = NULL;

//     sqlite3_prepare_v2_ExpectAndReturn(db, NULL, -1, NULL, NULL, SQLITE_ERROR);
//     sqlite3_errmsg_ExpectAndReturn(db, "Invalid database connection");

//     int result = load_scuderia_cars_from_database(scuderias, scuderia_count, &cars, db);

//     TEST_ASSERT_EQUAL_INT(-1, result);
//     TEST_ASSERT_NULL(cars);
// }

// int main(void)
// {
//   UNITY_BEGIN();
//   RUN_TEST(test_create_circuit_should_truncate_long_name);
//   return UNITY_END();
// }
