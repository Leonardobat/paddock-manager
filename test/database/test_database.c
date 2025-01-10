#include "unity.h"
#include <stdio.h>
#include "database/database.h"
#include <sqlite3.h>

void setUp(void) {};
void tearDown(void) {};

void test_connect_to_database_success(void)
{
    const char *test_db_filename = ":memory:";
    sqlite3 *db_connection = NULL;
    
    int result = connect_to_database(test_db_filename, &db_connection);
    
    TEST_ASSERT_EQUAL_INT(SQLITE_OK, result);
}

void test_connect_to_database_empty_filename(void)
{
    sqlite3 *db_connection = NULL;
    const char *empty_filename = "";

    int result = connect_to_database(empty_filename, &db_connection);

    TEST_ASSERT_EQUAL_INT(SQLITE_CANTOPEN, result);
}

void test_connect_to_database_null_filename(void)
{
    sqlite3 *database_connection = NULL;
    
    int result = connect_to_database(NULL, &database_connection);

    TEST_ASSERT_EQUAL_INT(SQLITE_CANTOPEN, result);
}


void test_connect_to_database_null_connection(void)
{
    const char *filename = ":memory:";
    
    int result = connect_to_database(filename, NULL);
   
    TEST_ASSERT_EQUAL_INT(SQLITE_CANTOPEN, result);
}



int main(void) {
  UNITY_BEGIN();
  RUN_TEST(test_connect_to_database_null_filename);
  RUN_TEST(test_connect_to_database_empty_filename);
  RUN_TEST(test_connect_to_database_success);
  RUN_TEST(test_connect_to_database_null_connection);
  return UNITY_END();
}