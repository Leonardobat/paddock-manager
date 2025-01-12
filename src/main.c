#include "car/car.h"
#include "circuit/circuit.h"
#include "circuit/sector.h"
#include "database/database.h"
#include "race_runner/racing_tyre.h"
#include "race_runner/runner.h"
#include "sqlite3.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

void create_test_database_entries(sqlite3 *db);

int main() {
  sqlite3 *db;
  int rc;

  // Connect to the database
  rc = connect_to_database(":memory:", &db);
  if (rc != SQLITE_OK) {
    fprintf(stderr, "Failed to connect to database: %s\n", sqlite3_errmsg(db));
    return 1;
  }

  // Migrate tables
  rc = migrate_tables(db);
  if (rc != SQLITE_OK) {
    fprintf(stderr, "Failed to migrate tables: %s\n", sqlite3_errmsg(db));
    sqlite3_close(db);
    return 1;
  }

  create_test_database_entries(db);

  // Load cars from the database
  Car *cars = NULL;
  char *scuderias[] = {"Scuderia Giordano"};
  int scuderia_count = sizeof(scuderias) / sizeof(scuderias[0]);
  int car_count =
      load_scuderia_cars_from_database(scuderias, scuderia_count, &cars, db);

  if (car_count <= 0) {
    fprintf(stderr, "Failed to load cars from the database\n");
    sqlite3_close(db);
    return 1;
  }

  // Load circuit from the database
  Circuit *myCircuit = NULL;
  rc = load_circuit_from_db(1, &myCircuit, db); // Assuming circuit with ID 1
  if (rc != SQLITE_OK || myCircuit == NULL) {
    fprintf(stderr, "Failed to load circuit from the database\n");
    free(cars);
    sqlite3_close(db);
    return 1;
  }

  // Create a racing tyre (it's not stored in the database yet, so we'll create it here)
  RacingTyre tyre = {
      .name = "Standard Racing Tyre",
      .critical_wear_level = 40,
      .wear_level = 100,
      .grip_level = 100,
  };

  // Run a simple simulation for each car
  for (int i = 0; i < car_count; i++) {
    RacingCar racingCar = {.car_info = &cars[i], .tyre = &tyre};

    printf("Simulating race for %s:\n", cars[i].scuderia);
    for (int lap = 0; lap < myCircuit->total_laps; ++lap) {
      bool pitstop = (lap % 10 == 0) && lap > 0;
      float_t lap_duration =
          calculate_lap_duration(&racingCar, myCircuit, pitstop);
      printf("  [%s] Lap [%d] duration: %f seconds\n", myCircuit->name, lap + 1,
             lap_duration);
    }
    printf("\n");
  }

  // Clean up
  destroy_circuit(myCircuit);
  free(cars);
  sqlite3_close(db);
  return 0;
}

void create_test_database_entries(sqlite3 *db) {
  Car myCar = {.scuderia = "Scuderia Giordano",
               .max_speed = 95,
               .downforce = 100,
               .acceleration = 100};

  Circuit myCircuit = {
      .id = 1,
      .name = "Test Circuit",
      .total_laps = 30,
      .pitstop_duration = 25,
      .num_sectors = 3,
      .psectors = (Sector[]){
          {.number = 1, .type = STRAIGHT, .length_in_meters = 2000},
          {.number = 2, .type = HIGH_SPEED_CORNERS, .length_in_meters = 2000},
          {.number = 3, .type = LOW_SPEED_CORNERS, .length_in_meters = 1500},
      }};

  // Insert car into the database
  save_cars_to_database(&myCar, 1, db);

  // Insert circuit into the database
  save_circuit_to_db(&myCircuit, db);
}
