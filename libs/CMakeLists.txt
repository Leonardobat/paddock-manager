add_subdirectory(sqlite3)

# Conditionally add Unity for testing
if(BUILD_TESTING)
    add_subdirectory(unity)
endif()