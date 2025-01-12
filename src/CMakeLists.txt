# Collect all the source files in the src directory
file(GLOB_RECURSE SRC_FILES *.c)

# Find GTK4 package
find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK4 REQUIRED gtk4)

# Create a library from the source files
add_library(paddock_manager_lib ${SRC_FILES})

# Add data dir for embed binaries
target_compile_options(paddock_manager_lib PRIVATE 
    "--embed-dir=${CMAKE_SOURCE_DIR}/data/"
)

# Include directories
target_include_directories(paddock_manager_lib 
    PUBLIC 
        ${CMAKE_SOURCE_DIR}/include
        ${GTK4_INCLUDE_DIRS} # Add GTK4 include directories
)

# Link libraries
target_link_libraries(paddock_manager_lib 
    PRIVATE 
        m 
        sqlite3 
        ${GTK4_LIBRARIES} # Link the GTK4 pkg-config target
)

# Add GTK4 compile options
target_compile_options(paddock_manager_lib PRIVATE ${GTK4_CFLAGS_OTHER})