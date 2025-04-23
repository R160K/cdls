cmake_minimum_required(VERSION 3.10)

# Get version from config.json using Python script
execute_process(
    COMMAND ${CMAKE_COMMAND} -E env python3 ${CMAKE_SOURCE_DIR}/get_version.py
    OUTPUT_VARIABLE PROJECT_VERSION
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

message(STATUS "Version from script: '${PROJECT_VERSION}'")

# Set the project name and version
project(cdls VERSION ${PROJECT_VERSION})

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Define a preprocessor macro with the version
add_compile_definitions(APP_VERSION="$PROJECT_VERSION")

# Add an executable
add_executable(cdls_ex ../src/main.cpp)

# Include directories (optional)
# target_include_directories(MyExecutable PUBLIC ${PROJECT_SOURCE_DIR}/include)

# Link libraries (optional)
# target_link_libraries(MyExecutable PRIVATE some_library)
