#ifndef CIRCUIT_H
#define CIRCUIT_H

#include "sector.h"
#include "sqlite3.h"
#include <stddef.h>
#include <stdint.h>

typedef struct {
  uint32_t id;
  char name[64];
  Sector *psectors;
  uint8_t num_sectors;
  uint8_t total_laps;
  uint8_t pitstop_duration;
} Circuit;

Circuit *create_circuit(const char *name, Sector *sectors, uint8_t num_sectors,
                        uint8_t total_laps, uint8_t pitstop_duration);

void destroy_circuit(Circuit *circuit);

int load_circuit_from_db(uint32_t id, Circuit **pcircuit, sqlite3 *db);

int save_circuit_to_db(Circuit *circuit, sqlite3 *db);

#endif // CIRCUIT_H