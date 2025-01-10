#include "circuit/circuit.h"
#include "circuit/sector.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Circuit *create_circuit(const char *name, Sector *sectors, uint8_t num_sectors,
                        uint8_t total_laps, uint8_t pitstop_duration) {
  if (name == NULL || sectors == NULL || num_sectors == 0 || total_laps == 0 ||
      pitstop_duration == 0) {
    return NULL;
  }
    // Check if each sector is valid
  for (uint8_t i = 0; i < num_sectors; i++) {
    if (!is_valid_sector(&sectors[i])) {
      return NULL;  // Invalid sector found
    }
  }

  Circuit *new_circuit = (Circuit *)malloc(sizeof(Circuit));
  if (new_circuit == NULL) {
    return NULL; // Memory allocation failed
  }

  // Copy the name (with bounds checking)
  strncpy(new_circuit->name, name, sizeof(new_circuit->name) - 1);
  new_circuit->name[sizeof(new_circuit->name) - 1] =
      '\0'; // Ensure null-termination

  new_circuit->psectors = sectors;
  new_circuit->num_sectors = num_sectors;
  new_circuit->total_laps = total_laps;
  new_circuit->pitstop_duration = pitstop_duration;

  return new_circuit;
}

void destroy_circuit(Circuit *circuit) {
  if (circuit != NULL) {
    if (circuit->psectors != NULL) {
      free(circuit->psectors);
      circuit->psectors = NULL;
    }
    free(circuit);
  }
}

int load_circuit_from_db(uint32_t id, Circuit **pcircuit, sqlite3 *db) {
  int result_code;
  if (db == NULL) {
    return -1;
  }

  const char *sql_basic =
      "SELECT id, name, "
      "       (SELECT COUNT(*) FROM json_each(sectors)) as num_sectors, "
      "       total_laps, pitstop_duration "
      "FROM circuit "
      "WHERE id = ?";

  sqlite3_stmt *stmt_basic;

  result_code = sqlite3_prepare_v2(db, sql_basic, -1, &stmt_basic, 0);
  if (result_code != SQLITE_OK) {
    fprintf(stderr, "Error while reading the circuit with id: %d, %s\n", id,
            sqlite3_errmsg(db));
    return result_code;
  }

  result_code = sqlite3_bind_int(stmt_basic, 1, id);

  if (sqlite3_step(stmt_basic) != SQLITE_ROW) {
    fprintf(stderr, "Error: No circuit found with id %d: %s\n", id,
            sqlite3_errmsg(db));
    sqlite3_finalize(stmt_basic);
    return SQLITE_ERROR;
  }

  uint8_t circuit_id = (uint8_t)sqlite3_column_int(stmt_basic, 0);
  const char *circuit_name = (const char *)sqlite3_column_text(stmt_basic, 1);
  uint8_t num_sectors = (uint8_t)sqlite3_column_int(stmt_basic, 2);
  uint8_t total_laps = (uint8_t)sqlite3_column_int(stmt_basic, 3);
  uint8_t pitstop_duration = (uint8_t)sqlite3_column_int(stmt_basic, 4);

  sqlite3_finalize(stmt_basic);

  Sector *psectors = malloc(num_sectors * sizeof(Sector));
  if (psectors == NULL) {
    fprintf(stderr, "Error: Memory allocation failed for sectors\n");
    return SQLITE_NOMEM;
  }

  const char *sql_sectors =
      "SELECT value ->> '$.number', "
      "       value ->> '$.type' as type, "
      "       value ->> '$.length_in_meters' as length_in_meters "
      "FROM circuit, json_each(circuit.sectors) "
      "WHERE circuit.id = ? "
      "ORDER BY number";

  sqlite3_stmt *stmt_sectors;

  result_code = sqlite3_prepare_v2(db, sql_sectors, -1, &stmt_sectors, NULL);
  if (result_code != SQLITE_OK) {
    fprintf(stderr, "Error preparing sector query for circuit id %d: %s\n", id,
            sqlite3_errmsg(db));
    free(psectors);
    return result_code;
  }

  result_code = sqlite3_bind_int(stmt_sectors, 1, id);
  if (result_code != SQLITE_OK) {
    fprintf(stderr, "Error binding circuit id %d to sector query: %s\n", id,
            sqlite3_errmsg(db));
    sqlite3_finalize(stmt_sectors);
    free(psectors);
    return result_code;
  }

  int sector_index = 0;
  while ((result_code = sqlite3_step(stmt_sectors)) == SQLITE_ROW &&
         sector_index < num_sectors) {
    psectors[sector_index].number =
        (uint8_t)sqlite3_column_int(stmt_sectors, 0);
    psectors[sector_index].type = (uint8_t)sqlite3_column_int(stmt_sectors, 1);
    psectors[sector_index].length_in_meters =
        (uint8_t)sqlite3_column_int(stmt_sectors, 2);
    sector_index++;
  }

  sqlite3_finalize(stmt_sectors);

  if (result_code != SQLITE_DONE) {
    fprintf(stderr, "Error reading sectors for circuit id %d: %s\n", id,
            sqlite3_errmsg(db));
    free(psectors);
    return result_code;
  }

  if (sector_index != num_sectors) {
    fprintf(stderr, "Error: Mismatch in number of sectors for circuit id %d\n",
            id);
    free(psectors);
    return SQLITE_ERROR;
  }

  *pcircuit = create_circuit(circuit_name, psectors, num_sectors, total_laps,
                             pitstop_duration);
  if (*pcircuit == NULL) {
    fprintf(stderr, "Error: Failed to create circuit\n");
    free(psectors);
    return SQLITE_ERROR;
  }

  (*pcircuit)->id = circuit_id; // Set the circuit ID
  return result_code;
}

int save_circuit_to_db(Circuit *circuit, sqlite3 *db) {
  if (circuit == NULL || db == NULL) {
    return -1;
  }

  const char *sql = "INSERT INTO circuit (id, name, sectors, total_laps, "
                    "pitstop_duration) VALUES (?, ?, jsonb(?), ?, ?)";
  sqlite3_stmt *stmt;

  int result_code = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
  if (result_code != SQLITE_OK) {
    fprintf(stderr, "SQL error: %s\n", sqlite3_errmsg(db));
    return result_code;
  }

  result_code = sqlite3_exec(db, "BEGIN TRANSACTION", NULL, NULL, NULL);
  if (result_code != SQLITE_OK) {
    fprintf(stderr, "Failed to begin transaction: %s\n", sqlite3_errmsg(db));
    sqlite3_finalize(stmt);
    return result_code;
  }

  // Bind the circuit name
  sqlite3_bind_int(stmt, 1, circuit->id);
  sqlite3_bind_text(stmt, 2, circuit->name, -1, SQLITE_STATIC);

  char *sectors_json = NULL;
  size_t sectors_json_len = 0;
  FILE *stream = open_memstream(&sectors_json, &sectors_json_len);

  if (stream == NULL) {
    sqlite3_finalize(stmt);
    return result_code;
  }

  fprintf(stream, "[");
  for (int i = 0; i < circuit->num_sectors; i++) {
    fprintf(stream,
            "{\"number\": %d, \"type\": %d, \"length_in_meters\": %d}%s",
            circuit->psectors[i].number, circuit->psectors[i].type,
            circuit->psectors[i].length_in_meters,
            (i < circuit->num_sectors - 1) ? "," : "");
  }
  fprintf(stream, "]");
  fclose(stream);

  // Bind the sectors JSON
  sqlite3_bind_text(stmt, 3, sectors_json, -1, SQLITE_STATIC);

  // Bind total_laps and pitstop_duration
  sqlite3_bind_int(stmt, 4, circuit->total_laps);
  sqlite3_bind_int(stmt, 5, circuit->pitstop_duration);

  result_code = sqlite3_step(stmt);
  if (result_code != SQLITE_DONE) {
    fprintf(stderr, "Execution failed: %s\n", sqlite3_errmsg(db));
    sqlite3_exec(db, "ROLLBACK", NULL, NULL, NULL);
    sqlite3_finalize(stmt);
    return result_code;
  }

  result_code = sqlite3_exec(db, "COMMIT", NULL, NULL, NULL);
  if (result_code != SQLITE_OK) {
    fprintf(stderr, "Failed to commit transaction: %s\n", sqlite3_errmsg(db));
    sqlite3_exec(db, "ROLLBACK", NULL, NULL, NULL);
    sqlite3_finalize(stmt);
    return result_code;
  }

  free(sectors_json);
  sqlite3_finalize(stmt);

  return result_code;
}