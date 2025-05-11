# Donation Management System

## Overview

The Donation Management System is a C++ console application designed for managing donations to charitable organizations. It supports user registration and login with role-based access (Administrator and User), enables charity management, facilitates donation processing, and features the generation of donation reports in PDF format. Key technologies include C++, CMake, nlohmann/json for JSON handling, bcrypt for password hashing, and wkhtmltopdf for PDF generation.

---

## Project Directory Structure

Project_ALL/
├── CMakeLists.txt
├── data/
│   ├── users.json
│   ├── donations.json
│   ├── charities.json
│   └── unilogo.png
├── MY_Pdfs/
├── output/
│   └── logs.json
├── src/
│   ├── main.cpp
│   ├── MY_hashingthingys/
│   │   └── bcrypt.h
│   │   └── bcrypt.cpp
│   ├── my_functions/
│   │   ├── user_management.cpp
│   │   ├── panels.cpp
│   │   ├── exporttopdf.cpp
│   │   └── ... (other .cpp files)
│   ├── nlohman/
│   │   └── json.hpp
│   ├── structsandfunctions/
│   │   ├── structures.h
│   │   └── functions.h
│   │   └── exporttopdf.h
│   └── utils/
│       ├── logger.h
│       └── logger.cpp
└── cmake-build-debug/ (or similar build output directory)

---

## Setup and Building Instructions

### Prerequisites

1. C++ Compiler (C++17 or newer, e.g., MinGW g++, Clang, MSVC).
2. CMake (Version 3.10 or higher).
3. wkhtmltopdf:
   Download from: https://wkhtmltopdf.org/downloads.html
   Ensure the wkhtmltopdf executable directory is added to your system's PATH.
4. (Optional) Git.

### Configuration (Critical)

Before building, update the absolute file paths in the following source files:

1. In `src/utils/logger.cpp`:
   Modify the `PROJECT_ROOT_FOR_JSON_LOG_PATHS` constant:
   const std::string PROJECT_ROOT_FOR_JSON_LOG_PATHS = "C:/full/path/to/your/Project_ALL/";

2. In `src/my_functions/panels.cpp` (within the PDF export section):
   Verify and update `project_root_path` if it's not derived from a global constant.
   Ensure `logo_file_path` correctly points to your `unilogo.png` (default: `project_root_path + "data/unilogo.png";`).

3. Review other source files (e.g., `user_management.cpp`, `exporttopdf.cpp`) for any other hardcoded paths and update them. It is recommended to centralize path management.

### Building the Project

1. Clone the repository (if applicable):
   git clone <repository-url>
   cd Project_ALL

2. Create a build directory:
   mkdir build
   cd build

3. Configure the project using CMake:
   cmake ..
   (For MinGW on Windows, you might need to specify: cmake .. -G "MinGW Makefiles")

4. Compile the project:
   cmake --build .
   (Or, if using Makefiles: make)

The executable will be located in the build directory (e.g., `build/your_project_name.exe`).

### Running the Application

Navigate to the build directory containing the executable and run it from your terminal:

For Linux/macOS:
./your_project_name

For Windows (Command Prompt or PowerShell):
.\your_project_name.exe

---