#include <stdlib.h>
#include <string.h>
#include "circuit/circuit.h"

Circuit* create_circuit(const char* name, Sector* sectors, uint8_t num_sectors) {
    if (name == NULL || sectors == NULL || num_sectors == 0) {
        return NULL; 
    }

    Circuit* new_circuit = (Circuit*)malloc(sizeof(Circuit));
    if (new_circuit == NULL) {
        return NULL;  // Memory allocation failed
    }

    // Copy the name (with bounds checking)
    strncpy(new_circuit->name, name, sizeof(new_circuit->name) - 1);
    new_circuit->name[sizeof(new_circuit->name) - 1] = '\0';  // Ensure null-termination

    new_circuit->psectors = sectors;
    new_circuit->num_sectors = num_sectors;

    return new_circuit;
}

void destroy_circuit(Circuit* circuit) {
    if (circuit != NULL) {
        free(circuit);
    }
}