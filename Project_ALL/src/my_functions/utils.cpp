#include "structsandfunctions/functions.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <ctime>

using namespace std;

datee getCurrentDateTime() {
    datee currentDateTimeVal;
    time_t rawTime;
    time(&rawTime);
    tm localTimeInfo;

#ifdef _WIN32
    errno_t err = localtime_s(&localTimeInfo, &rawTime);
    if (err != 0) {
        // Silently set default values on error
        currentDateTimeVal.date = "0000-00-00";
        currentDateTimeVal.time = "00:00:00";
        return currentDateTimeVal;
    }
#else
    if (localtime_r(&rawTime, &localTimeInfo) == nullptr) {
        currentDateTimeVal.date = "0000-00-00";
        currentDateTimeVal.time = "00:00:00";
        return currentDateTimeVal;
    }
#endif

    ostringstream dateStream;
    dateStream << (1900 + localTimeInfo.tm_year) << "-"
               << setw(2) << setfill('0') << (localTimeInfo.tm_mon + 1) << "-"
               << setw(2) << setfill('0') << localTimeInfo.tm_mday;

    ostringstream timeStream;
    timeStream << setw(2) << setfill('0') << localTimeInfo.tm_hour << ":"
               << setw(2) << setfill('0') << localTimeInfo.tm_min << ":"
               << setw(2) << setfill('0') << localTimeInfo.tm_sec;

    currentDateTimeVal.date = dateStream.str();
    currentDateTimeVal.time = timeStream.str();

    return currentDateTimeVal;
}