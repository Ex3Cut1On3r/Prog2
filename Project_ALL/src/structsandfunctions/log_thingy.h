#ifndef LOGGER_H
#define LOGGER_H
#include <string>
#include <filesystem>
enum class LogLevel { INFO, WARNING, ERR, DEBUG}; //enumerayion
void logMessage(LogLevel level, const std::string& message, bool alsoPrintToConsole = false);
#define LOG_INFO(message) logMessage(LogLevel::INFO, (message))
#define LOG_INFO_CONSOLE(message) logMessage(LogLevel::INFO, (message), true)
#define LOG_WARNING(message) logMessage(LogLevel::WARNING, (message))
#define LOG_WARNING_CONSOLE(message) logMessage(LogLevel::WARNING, (message), true)
#define LOG_ERR(message) logMessage(LogLevel::ERR, (message)) // Using ERR to avoid potential macro conflict
#define LOG_ERR_CONSOLE(message) logMessage(LogLevel::ERR, (message), true)
#define LOG_DEBUG(message) logMessage(LogLevel::DEBUG, (message))
#define LOG_DEBUG_CONSOLE(message) logMessage(LogLevel::DEBUG, (message), true)
#endif // LOGGER_H