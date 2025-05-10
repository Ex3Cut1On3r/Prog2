#include "structsandfunctions/log_thingy.h"
#include "nlohman/json.hpp" // Crucial for JSON operations
#include <iostream>
#include <fstream>
#include <filesystem>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <string>
#include <vector> // Might be useful if reading/writing JSON array directly

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#endif

using json = nlohmann::json;

// --- Path Constants (Defined here) ---
// **CRITICAL**: Update this path to the absolute root of your "Project_ALL" directory.
const std::string PROJECT_ROOT_FOR_JSON_LOG_PATHS = "C:/Users/Charlie/Documents/nigga/Programming2_Project-main/Project_ALL/"; // <<< UPDATE THIS!!!
const std::string JSON_LOG_OUTPUT_DIR_PATH = PROJECT_ROOT_FOR_JSON_LOG_PATHS + "output/"; // Output directory
const std::string JSON_LOG_FILE_PATH = JSON_LOG_OUTPUT_DIR_PATH + "logs.json"; // Target JSON log file

// --- Console Color Constants (Windows API - for console printing) ---
#ifdef _WIN32
const WORD JSON_LOG_CONSOLE_DEFAULT_COLOR = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
const WORD JSON_LOG_CONSOLE_INFO_COLOR = FOREGROUND_GREEN | FOREGROUND_BLUE;
const WORD JSON_LOG_CONSOLE_WARNING_COLOR = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY;
const WORD JSON_LOG_CONSOLE_ERROR_COLOR = FOREGROUND_RED | FOREGROUND_INTENSITY;
const WORD JSON_LOG_CONSOLE_DEBUG_COLOR = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;

void setJsonLoggerConsoleColor(WORD color) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, color);
}
#endif

std::string getCurrentTimestampForJsonLog() {
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);
    std::tm time_info_tm;
#ifdef _MSC_VER
    localtime_s(&time_info_tm, &in_time_t);
#else
    std::tm* p_time_info_tm = std::localtime(&in_time_t);
    if (p_time_info_tm) {
        time_info_tm = *p_time_info_tm;
    } else {
        time_info_tm = {};
    }
#endif
    std::ostringstream oss;
    oss << std::put_time(&time_info_tm, "%Y-%m-%d %H:%M:%S");
    return oss.str();
}

void logMessage(LogLevel level, const std::string& message, bool alsoPrintToConsole) {
    // 1. Ensure output directory exists
    try {
        if (!std::filesystem::exists(JSON_LOG_OUTPUT_DIR_PATH)) {
            if (!std::filesystem::create_directories(JSON_LOG_OUTPUT_DIR_PATH)) {
#ifdef _WIN32
                setJsonLoggerConsoleColor(JSON_LOG_CONSOLE_ERROR_COLOR);
#endif
                std::cerr << "CRITICAL LOG ERROR: Could not create log output directory: " << JSON_LOG_OUTPUT_DIR_PATH << std::endl;
#ifdef _WIN32
                setJsonLoggerConsoleColor(JSON_LOG_CONSOLE_DEFAULT_COLOR);
#endif
            }
        }
    } catch (const std::filesystem::filesystem_error& e) {
#ifdef _WIN32
        setJsonLoggerConsoleColor(JSON_LOG_CONSOLE_ERROR_COLOR);
#endif
        std::cerr << "CRITICAL LOG ERROR: Filesystem error checking/creating log output directory " << JSON_LOG_OUTPUT_DIR_PATH << ": " << e.what() << std::endl;
#ifdef _WIN32
        setJsonLoggerConsoleColor(JSON_LOG_CONSOLE_DEFAULT_COLOR);
#endif
    }

    // 2. Prepare log data as a JSON object
    std::string levelStr;
#ifdef _WIN32
    WORD consoleColor = JSON_LOG_CONSOLE_DEFAULT_COLOR;
#endif
    switch (level) {
        case LogLevel::INFO:    levelStr = "INFO";    
#ifdef _WIN32
                                consoleColor = JSON_LOG_CONSOLE_INFO_COLOR; 
#endif
                                break;
        case LogLevel::WARNING: levelStr = "WARNING"; 
#ifdef _WIN32
                                consoleColor = JSON_LOG_CONSOLE_WARNING_COLOR; 
#endif
                                break;
        case LogLevel::ERR:     levelStr = "ERROR";   
#ifdef _WIN32
                                consoleColor = JSON_LOG_CONSOLE_ERROR_COLOR; 
#endif
                                break;
        case LogLevel::DEBUG:   levelStr = "DEBUG";   
#ifdef _WIN32
                                consoleColor = JSON_LOG_CONSOLE_DEBUG_COLOR; 
#endif
                                break;
    }
    std::string timestamp = getCurrentTimestampForJsonLog();

    json logEntryJson;
    logEntryJson["timestamp"] = timestamp;
    logEntryJson["level"] = levelStr;
    logEntryJson["message"] = message;
    // You can add more fields here, e.g., "module", "function", "threadId", etc.

    // 3. Read existing logs, add new entry, and write back
    json logsArrayJson = json::array(); // Default to an empty array

    std::ifstream inFile(JSON_LOG_FILE_PATH);
    if (inFile.is_open()) {
        try {
            inFile >> logsArrayJson;
            if (!logsArrayJson.is_array()) { // Ensure it's an array
                logsArrayJson = json::array(); // If not, reset to empty
#ifdef _WIN32
                setJsonLoggerConsoleColor(JSON_LOG_CONSOLE_WARNING_COLOR);
#endif
                std::cerr << "WARNING LOG: " << JSON_LOG_FILE_PATH << " did not contain a valid JSON array. Starting fresh." << std::endl;
#ifdef _WIN32
                setJsonLoggerConsoleColor(JSON_LOG_CONSOLE_DEFAULT_COLOR);
#endif
            }
        } catch (json::parse_error& e) {
            // File exists but is not valid JSON or is empty
            logsArrayJson = json::array(); // Start with an empty array
#ifdef _WIN32
            setJsonLoggerConsoleColor(JSON_LOG_CONSOLE_WARNING_COLOR);
#endif
            std::cerr << "WARNING LOG: Could not parse " << JSON_LOG_FILE_PATH << ". Error: " << e.what() << ". Starting fresh." << std::endl;
#ifdef _WIN32
            setJsonLoggerConsoleColor(JSON_LOG_CONSOLE_DEFAULT_COLOR);
#endif
        }
        inFile.close();
    }
    // If file didn't exist, logsArrayJson is already an empty array.

    logsArrayJson.push_back(logEntryJson); // Add the new log entry

    std::ofstream outFile(JSON_LOG_FILE_PATH);
    if (!outFile.is_open()) {
#ifdef _WIN32
        setJsonLoggerConsoleColor(JSON_LOG_CONSOLE_ERROR_COLOR);
#endif
        std::cerr << "CRITICAL LOG ERROR: Failed to open log file for writing: " << JSON_LOG_FILE_PATH << std::endl;
#ifdef _WIN32
        setJsonLoggerConsoleColor(JSON_LOG_CONSOLE_DEFAULT_COLOR);
#endif
        // Optionally, print the log entry to console if file write fails and alsoPrintToConsole is true
        if (alsoPrintToConsole) {
#ifdef _WIN32
            setJsonLoggerConsoleColor(consoleColor);
#endif
            std::cerr << "(File Write Failed) " << timestamp << " [" << levelStr << "] " << message << std::endl;
#ifdef _WIN32
            setJsonLoggerConsoleColor(JSON_LOG_CONSOLE_DEFAULT_COLOR);
#endif
        }
        return;
    }

    try {
        outFile << std::setw(4) << logsArrayJson << std::endl; // Pretty print the JSON array
    } catch (const std::exception& e) {
#ifdef _WIN32
        setJsonLoggerConsoleColor(JSON_LOG_CONSOLE_ERROR_COLOR);
#endif
        std::cerr << "CRITICAL LOG ERROR: Failed to write JSON to log file: " << e.what() << std::endl;
#ifdef _WIN32
        setJsonLoggerConsoleColor(JSON_LOG_CONSOLE_DEFAULT_COLOR);
#endif
    }
    outFile.close();


    // 4. Optionally print to console (the original string message, not the JSON)
    if (alsoPrintToConsole) {
#ifdef _WIN32
        setJsonLoggerConsoleColor(consoleColor);
#endif
        if (level == LogLevel::ERR || level == LogLevel::WARNING) {
             std::cerr << timestamp << " [" << levelStr << "] " << message << std::endl;
        } else {
             std::cout << timestamp << " [" << levelStr << "] " << message << std::endl;
        }
#ifdef _WIN32
        setJsonLoggerConsoleColor(JSON_LOG_CONSOLE_DEFAULT_COLOR);
#endif
    }
}

/*
// If you had a version of logMessage that takes a json object:
void logMessage(LogLevel level, const json& jsonData, bool alsoPrintToConsole) {
    // Similar logic to above, but jsonData is already a json object.
    // You'd just add timestamp and level to it.
    json logEntryJson = jsonData; // Copy
    logEntryJson["timestamp"] = getCurrentTimestampForJsonLog();
    std::string levelStr;
    // ... set levelStr based on LogLevel level ...
    logEntryJson["level"] = levelStr;

    // ... rest of the logic to read, append, write JSON array ...
    // ... and optionally print to console (you'd decide how to print jsonData to console) ...
}
*/