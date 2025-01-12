#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <gtk/gtk.h>
#include "sqlite3.h"

GtkApplication * create_main_window(sqlite3 *database);

void set_car_text(const char * text);

void set_circuit_text(const char * text);

#endif // MAIN_WINDOW_H