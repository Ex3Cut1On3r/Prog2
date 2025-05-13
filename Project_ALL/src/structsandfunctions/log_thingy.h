#ifndef LOGGER_H
#define LOGGER_H
#include <string>
#include <filesystem>
enum class LogLevel { INFO, WARNING, ERR, DEBUG}; //enumerayion
void logMessage(LogLevel level, const std::string& message, bool alsoPrintToConsole = false);
#define LOG_INFO(message) logMessage(LogLevel::INFO, (message))
#endif // LOGGER_H