#ifndef SECTOR_H
#define SECTOR_H

#include "sector_type.h"
#include <stdint.h>

// Define the struct
typedef struct {
    uint8_t number;
    SectorType type;
    uint16_t length_in_meters;
} Sector;

#endif // SECTOR_H