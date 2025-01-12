#include "database/database.h"
#include <ctype.h>
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
  const char sql[] = {
    #embed <sql/tables.sql>
  };

  char *err_msg = 0;
  const char *tail = sql;

  while (*tail) {
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(pdatabase_connection, tail, -1, &stmt, &tail);

    if (rc != SQLITE_OK) {
      fprintf(stderr, "SQL error: %s\n", sqlite3_errmsg(pdatabase_connection));
      return rc;
    }

    if (stmt) {
      rc = sqlite3_step(stmt);
      if (rc != SQLITE_DONE) {
        fprintf(stderr, "Execution failed: %s\n",
                sqlite3_errmsg(pdatabase_connection));
        sqlite3_finalize(stmt);
        return rc;
      }
      sqlite3_finalize(stmt);
    }

    // Skip any whitespace or semicolons between statements
    while (*tail && (isspace((unsigned char)*tail) || *tail == ';')) {
      tail++;
    }
  }

  return SQLITE_OK;
}