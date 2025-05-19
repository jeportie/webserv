# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    CMakeLists.txt                                     :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: jeportie <jeportie@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/04/16 14:48:08 by jeportie          #+#    #+#              #
#    Updated: 2025/04/17 14:16:38 by jeportie         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

cmake_minimum_required(VERSION 3.10)
project(Interface LANGUAGES CXX)

# Set C++ standard and compilation flags
set(CMAKE_CXX_STANDARD 98)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "-Wall -Wextra -Werror -Wshadow")

# Find all source files for your main executable (excluding build directory)
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/src/*.cpp")
list(FILTER SOURCES EXCLUDE REGEX "^${CMAKE_SOURCE_DIR}/build/.*")

# Define main executable for your application
add_executable(${PROJECT_NAME} ${SOURCES} main.cpp)
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

### GOOGLE TEST INTEGRATION ###
enable_testing()

# Find GoogleTest package
find_package(GTest REQUIRED)
include(GoogleTest)

# Automatically collect all test files from the test/ folder matching *Test.cpp
file(GLOB_RECURSE TEST_FILES "${CMAKE_SOURCE_DIR}/test/*Test.cpp")

# Loop through each test file and create an executable target
foreach(test_src IN LISTS TEST_FILES)
    # Extract the base name (e.g., AnimalTest.cpp -> AnimalTest)
    get_filename_component(test_name ${test_src} NAME_WE)

    # Create an executable target for the test.
    # Optionally, include common sources if your tests rely on the code in src/.
    add_executable(${test_name} ${test_src} ${SOURCES})
    
    # Set the include directory so tests can access your headers
    target_include_directories(${test_name} PRIVATE ${CMAKE_SOURCE_DIR}/src)
    
    # Link the executable with GoogleTest libraries
    target_link_libraries(${test_name}
		GTest::GTest 
		GTest::gmock
		pthread
		GTest::Main)
    
    # Automatically discover tests within the built executable
    gtest_discover_tests(${test_name})
endforeach()
