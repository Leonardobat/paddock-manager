cmake_minimum_required(VERSION 3.30)
project(paddock-manager
    VERSION 0.0.1
    LANGUAGES C
)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED True)
set(CMAKE_EXPORT_COMPILE_COMMANDS True)

# Add the main project directories
add_subdirectory(include)
add_subdirectory(src)
add_executable(paddock-manager src/main.c)
target_link_libraries(paddock-manager PRIVATE paddock_manager_lib)

# Tests Configuration
# Add the Unity submodule directory
add_subdirectory(Unity)
# Enable testing
enable_testing()
# Add the test directory
add_subdirectory(test)

