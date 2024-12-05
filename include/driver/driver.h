#ifndef DRIVER_H
#define DRIVER_H

#include <stdint.h>

typedef struct {
    char name[64];
    uint8_t technique;
    uint8_t consistency;
    uint8_t overtaking;
} Driver;

#endif // DRIVER_H