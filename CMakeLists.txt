cmake_minimum_required(VERSION 3.10)
project(charles_project LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# --- Include Directories ---
# Add the 'src' directory of Project_ALL as an include path.
# This means when the compiler sees #include "...", it will look inside
# C:/Users/Charlie/Documents/nigga/Programming2_Project-main/Project_ALL/src/
include_directories(Project_ALL/src)

# --- Define the executable ---
# Paths to .cpp files are relative to this CMakeLists.txt file
add_executable(charles_project
        Project_ALL/src/main.cpp
        Project_ALL/src/my_functions/exporttopdf.cpp
        Project_ALL/src/my_functions/charity_management.cpp
        Project_ALL/src/my_functions/donation_management.cpp
        Project_ALL/src/my_functions/panels.cpp
        Project_ALL/src/my_functions/user_management.cpp
        Project_ALL/src/my_functions/utils.cpp
        Project_ALL/src/MY_hashingthingys/bcrypt.cpp
        Project_ALL/src/MY_hashingthingys/blowfish.cpp
        Project_ALL/src/my_functions/logger.cpp
)

# --- Linker Flags ---
if(WIN32)
    target_link_libraries(charles_project PRIVATE ws2_32)
endif()
if(UNIX AND NOT APPLE)
    target_compile_options(charles_project PRIVATE -pthread)
    target_link_libraries(charles_project PRIVATE pthread crypt)
endif()