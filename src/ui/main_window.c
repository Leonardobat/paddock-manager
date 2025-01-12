#include "ui/main_window.h"
#include "car/car.h"
#include "circuit/circuit.h"
#include "database/database.h"
#include "race_runner/racing_tyre.h"
#include "race_runner/runner.h"
#include <gtk/gtk.h>
#include <stdio.h>

static GtkWidget *window = NULL;
static GtkWidget *car_label = NULL;
static GtkWidget *circuit_label = NULL;
static GtkWidget *lap_label = NULL;
static GtkWidget *run_button = NULL;

static void run_simulation(GtkWidget *widget, sqlite3 *db);

static void activate(GtkApplication *app, sqlite3 *db) {
  GtkBuilder *builder;
  GError *error = NULL;

  // Load the UI file
  builder = gtk_builder_new();
  if (!gtk_builder_add_from_file(builder, "data/ui/io.github.paddock.ui",
                                 &error)) {
    g_printerr("Error loading UI file: %s\n", error->message);
    g_clear_error(&error);
    return;
  }

  // Get the main window
  window = GTK_WIDGET(gtk_builder_get_object(builder, "MainWindow"));
  if (!window) {
    g_printerr("Error: Could not find 'MainWindow' in the UI file\n");
    g_object_unref(builder);
    return;
  }
  gtk_window_set_application(GTK_WINDOW(window), app);

  // Get the labels and button
  car_label = GTK_WIDGET(gtk_builder_get_object(builder, "carLabel"));
  circuit_label = GTK_WIDGET(gtk_builder_get_object(builder, "circuitLabel"));
  lap_label = GTK_WIDGET(gtk_builder_get_object(builder, "lapLabel"));
  run_button = GTK_WIDGET(gtk_builder_get_object(builder, "runButton"));

  // Connect the button to the run_simulation function
  g_signal_connect(run_button, "clicked", G_CALLBACK(run_simulation), db);

  // Clean up
  g_object_unref(builder);

  gtk_widget_set_visible(window, TRUE);
}

GtkApplication *create_main_window(sqlite3 *database) {
  GtkApplication *app;
  app = gtk_application_new("io.github.paddock", G_APPLICATION_DEFAULT_FLAGS);
  g_signal_connect(app, "activate", G_CALLBACK(activate), database);
  return app;
}

void set_car_text(const char *text) {
  if (car_label) {
    gtk_label_set_text(GTK_LABEL(car_label), text);
  }
}

void set_circuit_text(const char *text) {
  if (circuit_label) {
    gtk_label_set_text(GTK_LABEL(circuit_label), text);
  }
}

static void set_laps_text(const char *text) {
  if (lap_label) {
    gtk_label_set_text(GTK_LABEL(lap_label), text);
  }
}

static void run_simulation(GtkWidget *widget, sqlite3 *db) {
  if (!db) {
    g_print("Database not initialized\n");
    return;
  }

  // Load circuit from the database
  Circuit *myCircuit = NULL;
  int rc =
      load_circuit_from_db(1, &myCircuit, db); // Assuming circuit with ID 1
  if (rc != SQLITE_OK || myCircuit == NULL) {
    g_print("Failed to load circuit from the database\n");
    return;
  }

  // Set the circuit text
  char circuit_text[255];
  snprintf(circuit_text, 255, "%s [%d Voltas]", myCircuit->name,
           myCircuit->total_laps);
  set_circuit_text(circuit_text);

  // Load cars from the database
  Car *cars = NULL;
  char *scuderias[] = {"Scuderia Giordano"};
  int scuderia_count = sizeof(scuderias) / sizeof(scuderias[0]);
  int car_count =
      load_scuderia_cars_from_database(scuderias, scuderia_count, &cars, db);

  if (car_count <= 0) {
    g_print("Failed to load cars from the database\n");
    return;
  }

  // Create a racing tyre
  RacingTyre tyre = {
      .name = "Standard Racing Tyre",
      .critical_wear_level = 40,
      .wear_level = 100,
      .grip_level = 100,
  };

  // Run a simple simulation for each car
  // TODO: make it run with call backs so it can actually update the UI
  for (int i = 0; i < car_count; i++) {
    Car car = cars[i];
    char car_text[255];
    snprintf(car_text, 255, "%s [Velocidade %.1f, Downforce %u, Aceleração %u]",
             car.scuderia, car.max_speed, car.downforce, car.acceleration);
    set_car_text(car_text);

    RacingCar racingCar = {.car_info = &car, .tyre = &tyre};
    for (int lap = 0; lap < myCircuit->total_laps; ++lap) {
      bool pitstop = (lap % 10 == 0) && lap > 0;
      float_t lap_duration =
          calculate_lap_duration(&racingCar, myCircuit, pitstop);
      char lap_text[64];
      snprintf(lap_text, 64, "%u", lap + 1);

      set_laps_text(lap_text);
      g_print("  [%s] Lap [%d] duration: %f seconds\n", myCircuit->name,
              lap + 1, lap_duration);
    }
    g_print("\n");
  }

  // Clean up
  destroy_circuit(myCircuit);
  free(cars);
}
