#include "database/database.h"
#include <stdio.h>

int connect_to_database(const char *pfilename, sqlite3 **pdatabase_connection) {
  if (pfilename == NULL || *pfilename == '\0' || pdatabase_connection == NULL) {
    fprintf(stderr, "Invalid arguments\n");
    return SQLITE_CANTOPEN;
  }

  int result_code = sqlite3_open(pfilename, pdatabase_connection);
  if (result_code != SQLITE_OK) {
    fprintf(stderr, "Can't open database: %s\n",
            sqlite3_errmsg(*pdatabase_connection));
  }
  return result_code;
}

int close_connection(sqlite3 *pdatabase_connection) {
  return sqlite3_close(pdatabase_connection);
}

int migrate_tables(sqlite3 *pdatabase_connection) {
  sqlite3_stmt *pStmt;
  const char sql[] = {
    #embed <sql/tables.sql>
  };

  int result_code =
      sqlite3_prepare_v2(pdatabase_connection, sql, -1, &pStmt, 0);
  if (result_code != SQLITE_OK) {
    fprintf(stderr, "SQLite3 prepare error: %s\n",
            sqlite3_errmsg(pdatabase_connection));
    return result_code;
  }

  result_code = sqlite3_step(pStmt);
  if (result_code != SQLITE_DONE) {
    fprintf(stderr, "SQL Error: %s\n", sqlite3_errmsg(pdatabase_connection));
    return result_code;
  }

  sqlite3_finalize(pStmt);
  return result_code;
}