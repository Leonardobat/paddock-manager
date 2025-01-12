#include "car/car.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

int load_scuderia_cars_from_database(char** scuderias, int scuderia_count, Car** cars, sqlite3 *db) {
    sqlite3_stmt *stmt;
    int rc;
    int car_count = 0;

    if (scuderia_count == 0) {
        return 0;
    }

    // Construct the SQL queries using parameter expansion
    const char *count_sql = "SELECT COUNT(*) FROM cars WHERE scuderia IN (";
    const char *select_sql = "SELECT id, scuderia, max_speed, acceleration, downforce FROM cars WHERE scuderia IN (";

    // Create a memory buffer for the parameter string
    char *params = NULL;
    size_t params_size = 0;
    FILE *stream = open_memstream(&params, &params_size);
    if (stream == NULL) {
        fprintf(stderr, "Failed to create memory stream\n");
        return -1;
    }

    // Write the parameter placeholders to the stream
    for (int i = 0; i < scuderia_count; i++) {
        fprintf(stream, "%s?", i > 0 ? "," : "");
    }
    fprintf(stream, ")");

    // Close the stream to finalize the string
    fclose(stream);

    // Create the full SQL statements
    char *full_count_sql = sqlite3_mprintf("%s%s", count_sql, params);
    char *full_select_sql = sqlite3_mprintf("%s%s", select_sql, params);

    // Free the params buffer as it's no longer needed
    free(params);

    // First, get the count of cars
    rc = sqlite3_prepare_v2(db, full_count_sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", sqlite3_errmsg(db));
        sqlite3_free(full_count_sql);
        sqlite3_free(full_select_sql);
        return -1;
    }

    for (int i = 0; i < scuderia_count; i++) {
        sqlite3_bind_text(stmt, i+1, scuderias[i], -1, SQLITE_STATIC);
    }

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        car_count = sqlite3_column_int(stmt, 0);
    }

    sqlite3_finalize(stmt);

    if (car_count == 0) {
        sqlite3_free(full_count_sql);
        sqlite3_free(full_select_sql);
        return 0;  // No cars found for these scuderias
    }

    // Allocate memory for the cars array
    *cars = (Car*)malloc(sizeof(Car) * car_count);
    if (*cars == NULL) {
        fprintf(stderr, "Memory allocation error\n");
        sqlite3_free(full_count_sql);
        sqlite3_free(full_select_sql);
        return -1;
    }

    // Now, load the actual car data
    rc = sqlite3_prepare_v2(db, full_select_sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", sqlite3_errmsg(db));
        free(*cars);
        *cars = NULL;
        sqlite3_free(full_count_sql);
        sqlite3_free(full_select_sql);
        return -1;
    }

    for (int i = 0; i < scuderia_count; i++) {
        sqlite3_bind_text(stmt, i+1, scuderias[i], -1, SQLITE_STATIC);
    }

    int index = 0;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        Car *car = &(*cars)[index];
        car->id = sqlite3_column_int(stmt, 0);
        strncpy(car->scuderia, (const char*)sqlite3_column_text(stmt, 1), sizeof(car->scuderia) - 1);
        car->scuderia[sizeof(car->scuderia) - 1] = '\0';  // Ensure null-termination
        car->max_speed = sqlite3_column_double(stmt, 2);
        car->acceleration = sqlite3_column_int(stmt, 3);
        car->downforce = sqlite3_column_int(stmt, 4);
        index++;
    }

    sqlite3_free(full_count_sql);
    sqlite3_free(full_select_sql);
    sqlite3_finalize(stmt);

    return car_count;
}

int save_cars_to_database(Car* cars, int car_count, sqlite3 *db) {
    sqlite3_stmt *stmt;
    int rc;
    const char *sql = "INSERT OR REPLACE INTO cars (id, scuderia, max_speed, acceleration, downforce) VALUES (?, ?, ?, ?, ?)";

    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", sqlite3_errmsg(db));
        return -1;
    }

    rc = sqlite3_exec(db, "BEGIN TRANSACTION", NULL, NULL, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to begin transaction: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return -1;
    }

    for (int i = 0; i < car_count; i++) {
        sqlite3_bind_int(stmt, 1, cars[i].id);
        sqlite3_bind_text(stmt, 2, cars[i].scuderia, -1, SQLITE_STATIC);
        sqlite3_bind_double(stmt, 3, cars[i].max_speed);
        sqlite3_bind_int(stmt, 4, cars[i].acceleration);
        sqlite3_bind_int(stmt, 5, cars[i].downforce);

        rc = sqlite3_step(stmt);
        if (rc != SQLITE_DONE) {
            fprintf(stderr, "Execution failed: %s\n", sqlite3_errmsg(db));
            sqlite3_exec(db, "ROLLBACK", NULL, NULL, NULL);
            sqlite3_finalize(stmt);
            return -1;
        }

        sqlite3_reset(stmt);
    }

    rc = sqlite3_exec(db, "COMMIT", NULL, NULL, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to commit transaction: %s\n", sqlite3_errmsg(db));
        sqlite3_exec(db, "ROLLBACK", NULL, NULL, NULL);
        sqlite3_finalize(stmt);
        return -1;
    }

    sqlite3_finalize(stmt);
    return car_count;
}
