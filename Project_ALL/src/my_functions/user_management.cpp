// File: Project_ALL/src/my_functions/user_management.cpp

// Define these BEFORE including windows.h to prevent conflicts
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX

#include "nlohman/json.hpp"
#include "MY_hashingthingys/bcrypt.h"
#include "structsandfunctions/structures.h"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <limits>
#include <string>
#include <vector>
#include <regex>
#include <chrono>
#include <ctime>
#include <sstream>
#include <windows.h> // For console colors - NOW AFTER THE DEFINES

using json = nlohmann::json;

const std::string USERS_JSON_FILE_PATH = "C:/Users/Charlie/Documents/nigga/Programming2_Project-main/Project_ALL/data/users.json";

const std::string LINE_SEPARATOR_THICK = "=========================================================";
const std::string LINE_SEPARATOR_THIN  = "---------------------------------------------------------";
const std::string HEADER_TOP_BOTTOM    = "+=======================================================+";
const std::string HEADER_SIDE          = "|";
const std::string PROMPT_ARROW         = "  ---> ";
const std::string SUCCESS_MARKER       = "[ OK ] ";
const std::string ERROR_MARKER         = "[FAIL] ";
const std::string WARNING_MARKER       = "[WARN] ";
const std::string INFO_MARKER          = "[INFO] ";
const std::string CRITICAL_MARKER      = "[CRIT] ";

const WORD DEFAULT_COLOR = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
const WORD HEADER_COLOR = FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY;
const WORD SUCCESS_COLOR = FOREGROUND_GREEN | FOREGROUND_INTENSITY;
const WORD ERROR_COLOR = FOREGROUND_RED | FOREGROUND_INTENSITY;
const WORD WARNING_COLOR = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY;
const WORD INFO_COLOR = FOREGROUND_BLUE | FOREGROUND_INTENSITY;
const WORD CRITICAL_COLOR = FOREGROUND_RED | FOREGROUND_INTENSITY;
const WORD PROMPT_COLOR = DEFAULT_COLOR;

void setConsoleColor(WORD color) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, color);
}

void printStyledHeader(const std::string& title) {
    setConsoleColor(HEADER_COLOR);
    std::cout << "\n" << HEADER_TOP_BOTTOM << std::endl;
    std::cout << HEADER_SIDE << std::left << std::setw(3) << "" << std::setw(52) << title << HEADER_SIDE << std::endl;
    std::cout << HEADER_TOP_BOTTOM << std::endl;
    setConsoleColor(DEFAULT_COLOR);
}

bool isValidEmailFormat(const std::string& email) {
    const std::regex pattern(R"(^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,}$)");
    return std::regex_match(email, pattern);
}

client getClientDataByEmail(const std::string& email) {
    client foundClient;
    foundClient.user_id = -1;
    std::ifstream inputFile(USERS_JSON_FILE_PATH);
    if (!inputFile.is_open()) {
        return foundClient;
    }
    json users_json_data;
    try {
        inputFile >> users_json_data;
        if (!users_json_data.is_array()) {
            inputFile.close();
            return foundClient;
        }
    } catch (const json::parse_error& e) {
        setConsoleColor(ERROR_COLOR);
        std::cerr << ERROR_MARKER << "getClientDataByEmail - JSON parse error: " << e.what() << std::endl;
        setConsoleColor(DEFAULT_COLOR);
        inputFile.close();
        return foundClient;
    } catch (const std::exception& e) {
        setConsoleColor(ERROR_COLOR);
        std::cerr << ERROR_MARKER << "getClientDataByEmail - Exception: " << e.what() << std::endl;
        setConsoleColor(DEFAULT_COLOR);
        inputFile.close();
        return foundClient;
    }
    inputFile.close();
    for (const auto& user_json_obj : users_json_data) {
        if (!user_json_obj.is_object() || !user_json_obj.contains("email")) {
            continue;
        }
        if (user_json_obj.value("email", "") == email) {
            try {
                foundClient.user_id = user_json_obj.value("user_id", -1);
                foundClient.first_name = user_json_obj.value("first_name", "");
                foundClient.last_name = user_json_obj.value("last_name", "");
                foundClient.email = user_json_obj.value("email", "");
                foundClient.phone = user_json_obj.value("phone", "");
                foundClient.password = user_json_obj.value("password", "");
                foundClient.role = user_json_obj.value("role", "user");
                if (user_json_obj.contains("number_ofdonations") && user_json_obj["number_ofdonations"].is_number_integer()) {
                    foundClient.number_ofdonations = user_json_obj["number_ofdonations"].get<int>();
                } else {
                    foundClient.number_ofdonations = 0;
                }
            } catch (const json::type_error& e) {
                setConsoleColor(WARNING_COLOR);
                std::cerr << WARNING_MARKER << "Type error for email " << email << ": " << e.what() << std::endl;
                setConsoleColor(DEFAULT_COLOR);
                foundClient.user_id = -1;
            }
            break;
        }
    }
    return foundClient;
}

client registerNewUser() {
    client newUser;
    std::string plain_password_input;
    json users_json_array = json::array();
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
    std::ostringstream time_ss;
    time_ss << std::put_time(&time_info_tm, "%I:%M %p");
    std::string current_time_str = time_ss.str();
    if (current_time_str.rfind("0", 0) == 0 && current_time_str.length() > 1 && std::isdigit(current_time_str[1])) {
         current_time_str.erase(0, 1);
    }
    std::string registration_title = "U S E R   R E G I S T R A T I O N   A T   " + current_time_str;
    printStyledHeader(registration_title);

    setConsoleColor(PROMPT_COLOR);
    std::cout << PROMPT_ARROW << "Enter First Name: ";
    setConsoleColor(DEFAULT_COLOR);
    std::getline(std::cin, newUser.first_name);

    setConsoleColor(PROMPT_COLOR);
    std::cout << PROMPT_ARROW << "Enter Last Name: ";
    setConsoleColor(DEFAULT_COLOR);
    std::getline(std::cin, newUser.last_name);

    while (true) {
        setConsoleColor(PROMPT_COLOR);
        std::cout << PROMPT_ARROW << "Enter Email: ";
        setConsoleColor(DEFAULT_COLOR);
        std::getline(std::cin, newUser.email);
        if (isValidEmailFormat(newUser.email)) {
            client existingUser = getClientDataByEmail(newUser.email);
            if (existingUser.user_id == -1) {
                break;
            } else {
                setConsoleColor(ERROR_COLOR);
                std::cout << "  " << ERROR_MARKER << "This email address is already registered.\n"
                          << "             Please use a different email or login.\n";
                setConsoleColor(DEFAULT_COLOR);
            }
        } else {
            setConsoleColor(ERROR_COLOR);
            std::cout << "  " << ERROR_MARKER << "Invalid email format. (e.g., user@example.com).\n";
            setConsoleColor(DEFAULT_COLOR);
        }
    }

    setConsoleColor(PROMPT_COLOR);
    std::cout << PROMPT_ARROW << "Enter Phone: ";
    setConsoleColor(DEFAULT_COLOR);
    std::getline(std::cin, newUser.phone);

    while(true){
        setConsoleColor(PROMPT_COLOR);
        std::cout << PROMPT_ARROW << "Enter password (min 8 characters): ";
        setConsoleColor(DEFAULT_COLOR);
        std::getline(std::cin, plain_password_input);
        if(plain_password_input.length() >= 8) {
            break;
        }
        setConsoleColor(ERROR_COLOR);
        std::cout << "  " << ERROR_MARKER << "Password too short. Please try again.\n";
        setConsoleColor(DEFAULT_COLOR);
    }
    newUser.password = bcrypt::generateHash(plain_password_input);

    setConsoleColor(PROMPT_COLOR);
    std::cout << PROMPT_ARROW << "Enter Role (user/admin - default is 'user'): ";
    setConsoleColor(DEFAULT_COLOR);
    std::getline(std::cin, newUser.role);
    if (newUser.role != "admin" && newUser.role != "user") {
        setConsoleColor(INFO_COLOR);
        std::cout << "  " << INFO_MARKER << "Invalid role. Defaulting to 'user'.\n";
        setConsoleColor(DEFAULT_COLOR);
        newUser.role = "user";
    }
    std::cout << LINE_SEPARATOR_THIN << std::endl;

    std::ifstream inputFile(USERS_JSON_FILE_PATH);
    if (inputFile.is_open()) {
        try {
            inputFile >> users_json_array;
            if (!users_json_array.is_array()) {
                 setConsoleColor(WARNING_COLOR);
                 std::cerr << WARNING_MARKER << "users.json is not JSON array. Starting fresh." << std::endl;
                 setConsoleColor(DEFAULT_COLOR);
                 users_json_array = json::array();
            }
        } catch (const json::parse_error&) {
            setConsoleColor(WARNING_COLOR);
            std::cerr << WARNING_MARKER << "Error parsing users.json. Will create/overwrite." << std::endl;
            setConsoleColor(DEFAULT_COLOR);
            users_json_array = json::array();
        }
        inputFile.close();
    } else {
        setConsoleColor(INFO_COLOR);
        std::cout << INFO_MARKER << "users.json not found. New file will be created." << std::endl;
        setConsoleColor(DEFAULT_COLOR);
    }

    int max_user_id = 0;
    for (const auto& user_json_obj : users_json_array) {
        if (user_json_obj.is_object() && user_json_obj.contains("user_id") && user_json_obj["user_id"].is_number_integer()) {
            if (user_json_obj["user_id"].get<int>() > max_user_id) {
                max_user_id = user_json_obj["user_id"].get<int>();
            }
        }
    }
    newUser.user_id = max_user_id + 1;
    newUser.number_ofdonations = 0;

    json user_json_to_add = {
        {"user_id", newUser.user_id},
        {"first_name", newUser.first_name},
        {"last_name", newUser.last_name},
        {"email", newUser.email},
        {"phone", newUser.phone},
        {"password", newUser.password},
        {"role", newUser.role},
        {"number_ofdonations", newUser.number_ofdonations}
    };
    users_json_array.push_back(user_json_to_add);

    std::ofstream outputFile(USERS_JSON_FILE_PATH);
    if (!outputFile.is_open()) {
        setConsoleColor(CRITICAL_COLOR);
        std::cerr << CRITICAL_MARKER << "Could not open '" << USERS_JSON_FILE_PATH << "' for writing! User not saved.\n";
        setConsoleColor(DEFAULT_COLOR);
        newUser.user_id = -1;
    } else {
        outputFile << std::setw(4) << users_json_array << std::endl;
        if (outputFile.fail()) {
             setConsoleColor(CRITICAL_COLOR);
             std::cerr << CRITICAL_MARKER << "Error writing to '" << USERS_JSON_FILE_PATH << "'. User not saved.\n";
             setConsoleColor(DEFAULT_COLOR);
             newUser.user_id = -1;
        } else {
            setConsoleColor(SUCCESS_COLOR);
            std::cout << LINE_SEPARATOR_THICK << std::endl;
            std::cout << SUCCESS_MARKER << "User '" << newUser.email << "' registered! ID: " << newUser.user_id << "\n";
            std::cout << LINE_SEPARATOR_THICK << std::endl;
            setConsoleColor(DEFAULT_COLOR);
        }
        outputFile.close();
    }
    return newUser;
}

bool verifyUserLogin(const std::string& email, const std::string& plain_password_input) {
    std::ifstream inputFile(USERS_JSON_FILE_PATH);
    if (!inputFile.is_open()) {
        return false;
    }
    json users_json_data;
    try {
        inputFile >> users_json_data;
        if (!users_json_data.is_array()) {
            inputFile.close();
            return false;
        }
    } catch (const std::exception&) {
        inputFile.close();
        return false;
    }
    inputFile.close();
    for (const auto& user_obj : users_json_data) {
        if (user_obj.is_object() && user_obj.contains("email") && user_obj["email"].is_string() &&
            user_obj.contains("password") && user_obj["password"].is_string()) {
             if (user_obj.value("email","") == email) {
                 std::string stored_hashed_password = user_obj.value("password","");
                 if (bcrypt::validatePassword(plain_password_input, stored_hashed_password)) {
                     return true;
                 }
                 return false;
             }
        }
    }
    return false;
}

void saveUserData(const client& userToSave) {
    json users_json_array;
    std::ifstream inputFile(USERS_JSON_FILE_PATH);
    if (inputFile.is_open()) {
        try {
            inputFile >> users_json_array;
            if (!users_json_array.is_array()) {
                setConsoleColor(WARNING_COLOR);
                std::cerr << WARNING_MARKER << "users.json not valid array. Save aborted for: " << userToSave.email << "." << std::endl;
                setConsoleColor(DEFAULT_COLOR);
                inputFile.close();
                return;
            }
        } catch (const json::parse_error& e) {
            setConsoleColor(WARNING_COLOR);
            std::cerr << WARNING_MARKER << "Error parsing users.json: " << e.what() << ". Save aborted for: " << userToSave.email << std::endl;
            setConsoleColor(DEFAULT_COLOR);
            inputFile.close();
            return;
        }
        inputFile.close();
    } else {
        setConsoleColor(INFO_COLOR);
        std::cout << INFO_MARKER << "users.json not found for save: " << userToSave.email << std::endl;
        setConsoleColor(DEFAULT_COLOR);
        return;
    }
    bool userFoundAndUpdated = false;
    for (auto& user_json_obj : users_json_array) {
        if (user_json_obj.is_object() && user_json_obj.contains("user_id") &&
            user_json_obj.value("user_id", -999) == userToSave.user_id) {
            user_json_obj["first_name"] = userToSave.first_name;
            user_json_obj["last_name"] = userToSave.last_name;
            user_json_obj["email"] = userToSave.email;
            user_json_obj["phone"] = userToSave.phone;
            user_json_obj["role"] = userToSave.role;
            user_json_obj["number_ofdonations"] = userToSave.number_ofdonations;
            userFoundAndUpdated = true;
            break;
        }
    }
    if (!userFoundAndUpdated) {
        setConsoleColor(CRITICAL_COLOR);
        std::cerr << CRITICAL_MARKER << "User ID " << userToSave.user_id << " (Email: " << userToSave.email
                  << ") NOT FOUND during save. Data NOT saved." << std::endl;
        setConsoleColor(DEFAULT_COLOR);
        return;
    }
    std::ofstream outputFile(USERS_JSON_FILE_PATH);
    if (!outputFile.is_open()) {
        setConsoleColor(CRITICAL_COLOR);
        std::cerr << CRITICAL_MARKER << "Could not open '" << USERS_JSON_FILE_PATH << "' for writing! User data for " << userToSave.email << " not saved.\n";
        setConsoleColor(DEFAULT_COLOR);
        return;
    }
    outputFile << std::setw(4) << users_json_array << std::endl;
    if (outputFile.fail()) {
        setConsoleColor(CRITICAL_COLOR);
        std::cerr << CRITICAL_MARKER << "Error writing user data to '" << USERS_JSON_FILE_PATH << "' for " << userToSave.email << ".\n";
        setConsoleColor(DEFAULT_COLOR);
    } else {
        setConsoleColor(INFO_COLOR);
        std::cout << INFO_MARKER << "User data for " << userToSave.email << " (ID: " << userToSave.user_id << ") saved." << std::endl;
        setConsoleColor(DEFAULT_COLOR);
    }
    outputFile.close();
}

bool checkIfAdmin(const std::string& email) {
    client user = getClientDataByEmail(email);
    return user.user_id != -1 && user.role == "admin";
}