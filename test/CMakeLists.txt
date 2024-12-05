# Get all test files in the current directory
file(GLOB_RECURSE TEST_SOURCES "test_*.c")

# Create a test executable for each test file
foreach(TEST_SOURCE ${TEST_SOURCES})
    # Get the filename without extension
    get_filename_component(TEST_NAME ${TEST_SOURCE} NAME_WE)
    
    # Add the test executable
    add_executable(${TEST_NAME} ${TEST_SOURCE})
    
    # Link the test executable with Unity and the main library
    target_link_libraries(${TEST_NAME} PRIVATE unity::framework paddock_manager_lib)
    
    # Add the test to CTest
    add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})
endforeach()