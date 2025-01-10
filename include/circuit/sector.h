#ifndef SECTOR_H
#define SECTOR_H

#include "sector_type.h"
#include <stdint.h>
#include <stdbool.h>

// Define the struct
typedef struct {
    uint8_t number;
    SectorType type;
    uint16_t length_in_meters;
} Sector;

bool is_valid_sector(const Sector *sector);

#endif // SECTOR_H