#ifndef CIRCUIT_H
#define CIRCUIT_H

#include "sector.h"
#include <stddef.h>
#include <stdint.h>

typedef struct {
    char name[64];
    Sector* psectors;
    uint8_t num_sectors;
    uint8_t total_laps;
    uint8_t pitstop_duration;
} Circuit;

Circuit* create_circuit(const char* name, Sector* sectors, uint8_t num_sectors, uint8_t total_laps, uint8_t pitstop_duration);

void destroy_circuit(Circuit* circuit);

#endif // CIRCUIT_H