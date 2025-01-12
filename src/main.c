#include "car/car.h"
#include "circuit/circuit.h"
#include "circuit/sector.h"
#include "database/database.h"
#include "sqlite3.h"
#include "ui/main_window.h"
#include <gtk/gtk.h>
#include <stdint.h>
#include <stdio.h>

void create_test_database_entries(sqlite3 *db);

int main(int argc, char **argv) {
  sqlite3 *db;

  // Connect to the database
  int rc = connect_to_database(":memory:", &db);
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
  GtkApplication *app = create_main_window(db);
  int status = g_application_run(G_APPLICATION(app), argc, argv);
  g_object_unref(app);
  sqlite3_close(db);
  return status;
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
