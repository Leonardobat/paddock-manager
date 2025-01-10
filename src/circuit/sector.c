#include "circuit/sector.h"
#include "circuit/sector_type.h"
#include <stdbool.h>
#include <string.h>

#define MIN_SECTOR_LENGHT 100
#define MAX_SECTOR_LENGHT 5000

static bool is_valid_sector_number(uint8_t number) { return number > 0; }

static bool is_valid_sector_type(SectorType type) {
  return (type >= LOW_SPEED_CORNERS && type <= STRAIGHT);
}

static bool is_valid_sector_length(uint16_t length_in_meters) {
  return (length_in_meters >= MIN_SECTOR_LENGHT &&
          length_in_meters <= MAX_SECTOR_LENGHT);
}

bool is_valid_sector(const Sector *sector) {
  if (sector == NULL) {
    return false;
  }

  return (is_valid_sector_number(sector->number) &&
          is_valid_sector_type(sector->type) &&
          is_valid_sector_length(sector->length_in_meters));
}