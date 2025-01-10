#ifndef CAR_H
#define CAR_H

#include <math.h>
#include <stdint.h>
#include "sqlite3.h"

typedef struct {
    uint32_t id;
    char scuderia[50];
    float_t max_speed;
    uint8_t acceleration;
    uint8_t downforce;
} Car;

int load_scuderia_cars_from_database(char** scuderias, int scuderia_count, Car** cars, sqlite3 *db);

int save_cars_to_database(Car* cars, int car_count, sqlite3 *db);
#endif // CAR_H