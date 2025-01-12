project(sqlite3 C)

## Download SQLite Almagmation Files if needed
add_custom_command(
    OUTPUT "${CMAKE_CURRENT_SOURCE_DIR}/sqlite3.c" "${CMAKE_CURRENT_SOURCE_DIR}/sqlite3.h" "${CMAKE_CURRENT_SOURCE_DIR}/sqlite3ext.h"
    COMMAND bash "${CMAKE_CURRENT_SOURCE_DIR}/download_sqlite.sh"
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMENT "Checking and downloading SQLite if necessary"
)

# Create SQLite library
add_library(sqlite3 STATIC
    sqlite3.c
)

add_custom_target(
    download_sqlite 
    DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/sqlite3.c" "${CMAKE_CURRENT_SOURCE_DIR}/sqlite3.h" "${CMAKE_CURRENT_SOURCE_DIR}/sqlite3ext.h"
    )
add_dependencies(sqlite3 download_sqlite)

# Set include directories for the library
target_include_directories(sqlite3 PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# SQLite needed libraries
find_package(Threads REQUIRED)
target_link_libraries(sqlite3 PRIVATE Threads::Threads m)

# Define compilation flags
target_compile_definitions(sqlite3 PRIVATE
    SQLITE_ENABLE_FTS5
    SQLITE_OMIT_DEPRECATED
    SQLITE_DQS=0
    SQLITE_STRICT_SUBTYPE=1
    SQLITE_OMIT_SHARED_CACHE
    SQLITE_USE_ALLOCA
    SQLITE_DEFAULT_WAL_SYNCHRONOUS=1
    SQLITE_LIKE_DOESNT_MATCH_BLOBS
    SQLITE_ENABLE_API_ARMOR
    SQLITE_ENABLE_MATH_FUNCTIONS
    SQLITE_ENABLE_SESSION
)