# Donation Management System

## Overview

The Donation Management System is a C++ console application designed for managing donations to charitable organizations. It supports user registration and login with role-based access (Administrator and User), enables comprehensive charity management (add, modify, remove, view), facilitates donation processing (make, view, modify, cancel), and features the generation of donation reports in PDF format for individual users.

Key technologies include C++ (C++17 standard), CMake for building, nlohmann/json for JSON data handling, a custom bcrypt implementation for secure password hashing, and wkhtmltopdf for converting HTML reports to PDF.

---

## Project Directory Structure


Project_ALL/
├── CMakeLists.txt # Main CMake build script
├── data/ # Directory for persistent data files
│ ├── users.json # Stores user account information and their donations
│ ├── donations.json # Global log of all donation transactions
│ ├── charities.json # Stores information about charities
│ └── unilogo.png # Logo used in PDF reports
├── MY_Pdfs/ # Default output directory for generated PDF reports
├── output/ # Default output directory for logs
│ └── logs.json # Application activity logs
├── src/ # Source code directory
│ ├── main.cpp # Main entry point of the application
│ ├── MY_hashingthingys/ # Password hashing utilities
│ │ ├── bcrypt.h
│ │ └── bcrypt.cpp
│ ├── my_functions/ # Core application logic modules
│ │ ├── user_management.cpp # User registration, login, data saving
│ │ ├── charity_management.cpp# Charity creation, modification, etc.
│ │ ├── donation_management.cpp# Donation processing
│ │ ├── panels.cpp # User interface panels (admin, user)
│ │ ├── exporttopdf.cpp # HTML and PDF generation logic
│ │ └── utils.cpp # Utility functions (e.g., date/time)
│ ├── nlohman/ # nlohmann/json library (single header)
│ │ └── json.hpp
│ ├── structsandfunctions/ # Structure definitions and function declarations
│ │ ├── structures.h # Defines core data structures (client, charity, etc.)
│ │ ├── functions.h # Central header for function declarations
│ │ ├── exporttopdf.h # Declarations for PDF export functions
│ │ └── log_thingy.h # Declarations for logging (if separated)
│ └── logger.cpp # (If logging implementation is separate from utils.cpp or log_thingy.h)
└── cmake-build-debug/ # Example build output directory (CLion specific)
└── (or your chosen build directory e.g., 'build/')

---

## Setup and Building Instructions

### Prerequisites

1.  **C++ Compiler:** A C++17 compliant compiler (e.g., MinGW g++ on Windows, Clang on macOS/Linux, MSVC on Windows).
2.  **CMake:** Version 3.10 or higher. ([Download CMake](https://cmake.org/download/))
3.  **wkhtmltopdf:**
   *   This is essential for PDF generation.
   *   Download from: [wkhtmltopdf.org/downloads.html](https://wkhtmltopdf.org/downloads.html)
   *   **Crucial:** Ensure the directory containing `wkhtmltopdf.exe` (on Windows) or `wkhtmltopdf` (on Linux/macOS) is added to your system's **PATH environment variable**.
4.  **(Optional) Git:** For cloning the repository.

### Configuration: Absolute Paths (IMPORTANT!)

This project, in its current state, uses **absolute file paths** in several C++ source files. You **MUST** update these paths to match the location of `Project_ALL/` on **your** system before the application can correctly access data files and generate outputs.

**Replace `"C:/Your/Full/Path/To/Project_ALL/"` in the examples below with the actual full path to your `Project_ALL` directory.**

**Files to check and update paths in:**

1.  **`src/my_functions/user_management.cpp`**:
   *   `USERS_JSON_FILE_PATH`:
      ```cpp
      const string USERS_JSON_FILE_PATH = "C:/Your/Full/Path/To/Project_ALL/data/users.json";
      ```

2.  **`src/my_functions/charity_management.cpp`**:
   *   `filePath` variable inside `loadCharities()` and `saveCharities()`:
      ```cpp
      string filePath = "C:/Your/Full/Path/To/Project_ALL/data/charities.json";
      ```

3.  **`src/my_functions/donation_management.cpp`**:
   *   `filePath` variable inside functions like `appendDonationToGlobalFile`, `loadDonationsFile`, `saveDonationsFile`:
      ```cpp
      string filePath = "C:/Your/Full/Path/To/Project_ALL/data/donations.json";
      ```

4.  **`src/my_functions/panels.cpp` (PDF Export Section - Case 6 in `userPanel`)**:
   *   `project_root_path`:
      ```cpp
      string project_root_path = "C:/Your/Full/Path/To/Project_ALL/";
      ```

5.  **`src/logger.cpp` (or where `PROJECT_ROOT_FOR_JSON_LOG_PATHS` is defined)**:
   *   `PROJECT_ROOT_FOR_JSON_LOG_PATHS`:
      ```cpp
      const string PROJECT_ROOT_FOR_JSON_LOG_PATHS = "C:/Your/Full/Path/To/Project_ALL/";
      ```

**Recommendation:** For future development, consider implementing a more robust path management strategy (e.g., paths relative to the executable, or paths configured at CMake time) to avoid hardcoding absolute paths.

### Building the Project

1.  **Clone the repository (if you haven't):**
    ```bash
    git clone <your-repository-url>
    cd Project_ALL
    ```

2.  **Create and navigate to a build directory:**
    ```bash
    mkdir build
    cd build
    ```

3.  **Configure the project using CMake:**
    From within the `build` directory:
    ```bash
    cmake ..
    ```
   *   If using MinGW on Windows:
       ```bash
       cmake .. -G "MinGW Makefiles"
       ```

4.  **Compile the project:**
    From within the `build` directory:
   *   If using Makefiles:
       ```bash
       make 
       ```
       (or `mingw32-make` on Windows)
   *   Generic CMake build command:
       ```bash
       cmake --build .
       ```

The executable (e.g., `charles_project.exe` or `charles_project`) will be in the `build` directory or a subdirectory.

### Running the Application

1.  **Navigate to the directory containing the executable** (usually your `build` directory).
2.  **Run it from your terminal:**

   *   **Windows (Command Prompt or PowerShell):**
       ```cmd
       .\your_project_name.exe
       ```
   *   **Linux/macOS:**
       ```bash
       ./your_project_name
       ```

**Note on Running:** Ensure the absolute paths you configured point correctly to your `data/` directory.

---

## Features

*   **User Authentication:** Secure registration and login using bcrypt.
*   **Role-Based Access:** Administrator and User roles.
*   **Charity Management:** Admins can add, modify, remove, and view charities (with "Active"/"Closed" status).
*   **Donation Processing:** Users can make donations, view their history, and modify/cancel donations.
*   **PDF Report Generation:** Users can export their donation history to PDF via `wkhtmltopdf`.
*   **Data Persistence:** User, charity, and donation data stored in JSON files.
*   **Logging:** Application activity logged to `output/logs.json`.

---

## Troubleshooting

*   **"Procedure entry point ... could not be located..." (Windows):**
   *   Run from your MinGW environment's terminal (e.g., CLion's terminal).
   *   Or, copy `libstdc++-6.dll`, `libgcc_s_seh-1.dll` (or `libgcc_s_dw2-1.dll`), and `libwinpthread-1.dll` (if using C++ threads) from your compiler's `bin` directory to your executable's directory.
*   **"File not found" errors for `.json` files:** Verify the absolute paths in the source code (see "Configuration" section).
*   **PDF generation fails:**
   *   Ensure `wkhtmltopdf` is installed and its directory is in your system PATH.
   *   Test `wkhtmltopdf` from the command line (e.g., `wkhtmltopdf http://google.com google.pdf`).

---
IGNORE_WHEN_COPYING_START
content_copy
download
Use code with caution.
IGNORE_WHEN_COPYING_END