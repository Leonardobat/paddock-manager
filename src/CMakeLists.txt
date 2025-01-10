# Collect all the source files in the src directory
file(GLOB_RECURSE SRC_FILES *.c)

# Create a library from the source files
add_library(paddock_manager_lib ${SRC_FILES})

# Add data dir for embed binaries
target_compile_options(paddock_manager_lib PRIVATE 
    "--embed-dir=${CMAKE_CURRENT_SOURCE_DIR}/../data/"
)

# Include directories if needed
target_include_directories(paddock_manager_lib 
    PUBLIC 
        ${CMAKE_CURRENT_SOURCE_DIR}/../include
)

# Link math library
target_link_libraries(paddock_manager_lib PRIVATE m)
target_link_libraries(paddock_manager_lib PRIVATE sqlite3)