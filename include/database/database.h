#include "sqlite3.h"

int connect_to_database(const char *pfilename, sqlite3** pdatabase_connection);

int close_connection(sqlite3* pdatabase_connection);

int migrate_tables(sqlite3* pdatabase_connection);