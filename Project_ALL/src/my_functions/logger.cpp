// File: Project_ALL/src/my_functions/logger.cpp

#include "structsandfunctions/log_thingy.h"
#include "nlohman/json.hpp"
#include <iostream>
#include <fstream>
#include <filesystem>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <string>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#endif
using namespace std;
using json = nlohmann::json;
const string PROJECT_ROOT_FOR_JSON_LOG_PATHS = "C:/Users/Charlie/Documents/ProgrammingTwoProject/Programming2_Project-main/Project_ALL/";
const string JSON_LOG_OUTPUT_DIR_PATH = PROJECT_ROOT_FOR_JSON_LOG_PATHS + "output/";
const string JSON_LOG_FILE_PATH = JSON_LOG_OUTPUT_DIR_PATH + "logs.json";
#ifdef _WIN32
const WORD JSON_LOG_CONSOLE_DEFAULT_COLOR = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
const WORD JSON_LOG_CONSOLE_INFO_COLOR = FOREGROUND_GREEN | FOREGROUND_BLUE;
const WORD JSON_LOG_CONSOLE_WARNING_COLOR = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY;
const WORD JSON_LOG_CONSOLE_ERROR_COLOR = FOREGROUND_RED | FOREGROUND_INTENSITY;
const WORD JSON_LOG_CONSOLE_DEBUG_COLOR = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE; // Was same as default, kept it

void setJsonLoggerConsoleColor(WORD color) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, color);
}
#endif

string getCurrentTimestampForJsonLog() {
    auto now = chrono::system_clock::now();
    auto in_time_t = chrono::system_clock::to_time_t(now);
    tm time_info_tm;
#if defined(_MSC_VER) || defined(__MINGW32__) || defined(__MINGW64__) // More robust check for MSVC/MinGW
    localtime_s(&time_info_tm, &in_time_t);
#else
    tm* p_time_info_tm = localtime(&in_time_t);
    if (p_time_info_tm) {
        time_info_tm = *p_time_info_tm;
    } else {
        time_info_tm = {}; // Default initialize
    }
#endif
    ostringstream oss;
    oss << put_time(&time_info_tm, "%Y-%m-%d %H:%M:%S");
    return oss.str();
}

void logMessage(LogLevel level, const string& message, bool alsoPrintToConsole) {
    try {
        if (!filesystem::exists(JSON_LOG_OUTPUT_DIR_PATH)) {
            if (!filesystem::create_directories(JSON_LOG_OUTPUT_DIR_PATH)) {
#ifdef _WIN32
                setJsonLoggerConsoleColor(JSON_LOG_CONSOLE_ERROR_COLOR);
#endif
                cout << "Could not create log output directory: " << JSON_LOG_OUTPUT_DIR_PATH << endl;
#ifdef _WIN32
                setJsonLoggerConsoleColor(JSON_LOG_CONSOLE_DEFAULT_COLOR);
#endif
                // Optionally, you might want to return or throw here if directory creation is critical
            }
        }
    } catch (const filesystem::filesystem_error& e) {
#ifdef _WIN32
        setJsonLoggerConsoleColor(JSON_LOG_CONSOLE_ERROR_COLOR);
#endif
        cout << "Filesystem error checking/creating log output directory " << JSON_LOG_OUTPUT_DIR_PATH << ": " << e.what() << endl;
#ifdef _WIN32
        setJsonLoggerConsoleColor(JSON_LOG_CONSOLE_DEFAULT_COLOR);
#endif
        // Optionally, return or throw
    }

    string levelStr;
#ifdef _WIN32
    WORD consoleColor = JSON_LOG_CONSOLE_DEFAULT_COLOR;
#endif
    switch (level) {
        case LogLevel::INFO:
            levelStr = "INFO";
#ifdef _WIN32
            consoleColor = JSON_LOG_CONSOLE_INFO_COLOR;
#endif
            break;
        case LogLevel::WARNING:
            levelStr = "WARNING";
#ifdef _WIN32
            consoleColor = JSON_LOG_CONSOLE_WARNING_COLOR;
#endif
            break;
        case LogLevel::ERR: // Matched to your enum
            levelStr = "ERROR";
#ifdef _WIN32
            consoleColor = JSON_LOG_CONSOLE_ERROR_COLOR;
#endif
            break;
        case LogLevel::DEBUG:
            levelStr = "DEBUG";
#ifdef _WIN32
            consoleColor = JSON_LOG_CONSOLE_DEBUG_COLOR;
#endif
            break;
        default: // Good practice to have a default
            levelStr = "UNKNOWN";
            break;
    }
    string timestamp = getCurrentTimestampForJsonLog();

    json logEntryJson;
    logEntryJson["timestamp"] = timestamp;
    logEntryJson["level"] = levelStr;
    logEntryJson["message"] = message;

    json logsArrayJson = json::array();

    ifstream inFile(JSON_LOG_FILE_PATH);
    if (inFile.is_open()) {
        try {
            inFile >> logsArrayJson;
            if (!logsArrayJson.is_array()) {
                logsArrayJson = json::array(); // Reset if not an array
#ifdef _WIN32
                setJsonLoggerConsoleColor(JSON_LOG_CONSOLE_WARNING_COLOR);
#endif
                cout << JSON_LOG_FILE_PATH << " did not contain a valid JSON array. Starting fresh." << endl;
#ifdef _WIN32
                setJsonLoggerConsoleColor(JSON_LOG_CONSOLE_DEFAULT_COLOR);
#endif
            }
        } catch (json::parse_error& e) {
            logsArrayJson = json::array(); // Reset on parse error
#ifdef _WIN32
            setJsonLoggerConsoleColor(JSON_LOG_CONSOLE_WARNING_COLOR);
#endif
            cout << "Could not parse " << JSON_LOG_FILE_PATH << ". Error: " << e.what() << ". Starting fresh." << endl;
#ifdef _WIN32
            setJsonLoggerConsoleColor(JSON_LOG_CONSOLE_DEFAULT_COLOR);
#endif
        }
        inFile.close();
    } // End of if (inFile.is_open())

    logsArrayJson.push_back(logEntryJson);

    ofstream outFile(JSON_LOG_FILE_PATH);
    if (!outFile.is_open()) {
#ifdef _WIN32
        setJsonLoggerConsoleColor(JSON_LOG_CONSOLE_ERROR_COLOR);
#endif
        cout << "Failed to open log file for writing: " << JSON_LOG_FILE_PATH << endl;
#ifdef _WIN32
        setJsonLoggerConsoleColor(JSON_LOG_CONSOLE_DEFAULT_COLOR);
#endif
        if (alsoPrintToConsole) {
#ifdef _WIN32
            setJsonLoggerConsoleColor(consoleColor);
#endif
            cout << "(File Write Failed) " << timestamp << " [" << levelStr << "] " << message << endl;
#ifdef _WIN32
            setJsonLoggerConsoleColor(JSON_LOG_CONSOLE_DEFAULT_COLOR);
#endif
        }
        return; // Exit if cannot write to log file
    }

    try {
        outFile << setw(4) << logsArrayJson << endl;
    } catch (const exception& e) { // Catch std::exception for broader error handling
#ifdef _WIN32
        setJsonLoggerConsoleColor(JSON_LOG_CONSOLE_ERROR_COLOR);
#endif
        cout << "Failed to write JSON to log file: " << e.what() << endl;
#ifdef _WIN32
        setJsonLoggerConsoleColor(JSON_LOG_CONSOLE_DEFAULT_COLOR);
#endif
        // outFile will be closed even if this fails due to RAII or explicit close
    }
    outFile.close();

    if (alsoPrintToConsole) {
#ifdef _WIN32
        setJsonLoggerConsoleColor(consoleColor);
#endif
        cout << timestamp << " [" << levelStr << "] " << message << endl;
#ifdef _WIN32
        setJsonLoggerConsoleColor(JSON_LOG_CONSOLE_DEFAULT_COLOR);
#endif
    }
} // This is the correct closing brace for logMessage function