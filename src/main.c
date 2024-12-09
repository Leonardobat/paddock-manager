#include "car/car.h"
#include "circuit/circuit.h"
#include "race_runner/racing_tyre.h"
#include "race_runner/runner.h"
#include "sqlite3.h"
#include <stdint.h>
#include <stdio.h>

int main() {
  Car myCar = {.scuderia = "Scuderia Giordano",
               .max_speed = 95,
               .downforce = 100,
               .acceleration = 100};
  RacingTyre tyre = {
      .name = "Standard Racing Tyre",
      .critical_wear_level = 40,
      .wear_level = 100,
      .grip_level = 100,
  };

  RacingCar myRacingCar = {.car_info = &myCar, .tyre = &tyre};

  Sector sectors[] = {
      {1, LOW_SPEED_CORNERS, 1500},
      {2, HIGH_SPEED_CORNERS, 1500},
      {3, STRAIGHT, 1500},
  };
  uint8_t num_sectors = sizeof(sectors) / sizeof(sectors[0]);

  Circuit *myCircuit =
      create_circuit("Circuit 1", sectors, num_sectors, 30, 20);
  if (myCircuit == NULL) {
    printf("Failed to create circuit\n");
    return 1;
  }

  for (int i = 0; i < myCircuit->total_laps; ++i) {
    bool pitstop = (i % 10 == 0) && i > 0;
    float_t lap_duration =
        calculate_lap_duration(&myRacingCar, myCircuit, pitstop);
    printf("[%s] Lap [%d] duration for %s: %f seconds\n", myCircuit->name,
           i + 1, myCar.scuderia, lap_duration);
  }

  destroy_circuit(myCircuit);

  // Test my SQLite compilation
  // Test SQLite functionality
  sqlite3 *db;
  sqlite3_stmt *pStmt;
  int rc;

  // Open database
  rc = sqlite3_open("test.db", &db);
  if (rc) {
    fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
    return (1);
  }

  // Create table
  const char *sql =
      "CREATE TABLE IF NOT EXISTS cars (id INTEGER PRIMARY KEY, name TEXT, scuderia TEXT)";
  rc = sqlite3_prepare_v2(db, sql, -1, &pStmt, 0);
  if (rc != SQLITE_OK) {
    fprintf(stderr, "SQLite3 prepare error: %s\n", sqlite3_errmsg(db));
    return (1);
  }

  rc = sqlite3_step(pStmt);
  if (rc != SQLITE_OK) {
    fprintf(stderr, "SQL Error: %s\n", sqlite3_errmsg(db));
    return (1);
  }
  sqlite3_finalize(pStmt);

  // Insert a record
  sql = "INSERT INTO cars (name, scuderia) VALUES (?, ?)";
  rc = sqlite3_prepare_v2(db, sql, -1, &pStmt, 0);
  if (rc == SQLITE_OK) {
    sqlite3_bind_text(pStmt, 1, "MyRaceCar", -1, SQLITE_STATIC);
    sqlite3_bind_text(pStmt, 2, myCar.scuderia, -1, SQLITE_STATIC);
    sqlite3_step(pStmt);
  }
  sqlite3_finalize(pStmt);

  // Read and print the record
  sql = "SELECT * FROM cars";
  rc = sqlite3_prepare_v2(db, sql, -1, &pStmt, 0);
  if (rc == SQLITE_OK) {
    while (sqlite3_step(pStmt) == SQLITE_ROW) {
      int id = sqlite3_column_int(pStmt, 0);
      const unsigned char *name = sqlite3_column_text(pStmt, 1);
      const unsigned char *scuderia = sqlite3_column_text(pStmt, 2);
      printf("Car: ID = %d, Name = %s, Scuderia = %s\n", id, name, scuderia);
    }
  }
  sqlite3_finalize(pStmt);

  // Close database
  sqlite3_close(db);
  return 0;
}
