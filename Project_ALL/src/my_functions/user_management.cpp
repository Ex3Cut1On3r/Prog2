#define WIN32_LEAN_AND_MEAN
#ifndef NOMINMAX
#define NOMINMAX
#endif

#include <windows.h>
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
#include <algorithm>

#include "nlohman/json.hpp"
#include "MY_hashingthingys/bcrypt.h"
#include "structsandfunctions/structures.h"
#include "structsandfunctions/functions.h"

using json = nlohmann::json;
using namespace std;

const string USERS_JSON_FILE_PATH = "C:/Users/Charlie/Documents/ProgrammingTwoProject/Programming2_Project-main/Project_ALL/data/users.json";

const string LINE_SEPARATOR_THICK = "=========================================================";
const string LINE_SEPARATOR_THIN  = "---------------------------------------------------------";
const string HEADER_TOP_BOTTOM    = "+=======================================================+";
const string HEADER_SIDE          = "|";
const string PROMPT_ARROW         = "  ---> ";
const string SUCCESS_MARKER       = "[ OK ] ";
const string ERROR_MARKER         = "[FAIL] ";
const string WARNING_MARKER       = "[WARN] ";
const string INFO_MARKER          = "[INFO] ";
const string CRITICAL_MARKER      = "[CRIT] ";

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

void printStyledHeader(const string& title) {
    setConsoleColor(HEADER_COLOR);
    cout << "\n" << HEADER_TOP_BOTTOM << endl;
    cout << HEADER_SIDE << left << setw(3) << "" << setw(52) << title << HEADER_SIDE << endl;
    cout << HEADER_TOP_BOTTOM << endl;
    setConsoleColor(DEFAULT_COLOR);
}

bool isValidEmailFormat(const string& email) {
    const regex pattern(R"(^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,}$)");
    return regex_match(email, pattern);
}

client getClientDataByEmail(const string& email) {
    client foundClient;
    foundClient.user_id = -1;
    foundClient.donations = nullptr;
    foundClient.number_ofdonations = 0;

    ifstream inputFile(USERS_JSON_FILE_PATH);
    if (!inputFile.is_open()) {
        setConsoleColor(ERROR_COLOR);
        cout << ERROR_MARKER << "getClientDataByEmail - Could not open users file: " << USERS_JSON_FILE_PATH << endl;
        setConsoleColor(DEFAULT_COLOR);
        return foundClient;
    }

    json users_json_data;
    try {
        inputFile >> users_json_data;
        if (!users_json_data.is_array()) {
            setConsoleColor(ERROR_COLOR);
            cout << ERROR_MARKER << "getClientDataByEmail - User data is not a JSON array." << endl;
            setConsoleColor(DEFAULT_COLOR);
            inputFile.close();
            return foundClient;
        }
    } catch (const json::parse_error& e) {
        setConsoleColor(ERROR_COLOR);
        cout << ERROR_MARKER << "getClientDataByEmail - JSON parse error: " << e.what() << endl;
        setConsoleColor(DEFAULT_COLOR);
        inputFile.close();
        return foundClient;
    } catch (const std::exception& e) {
        setConsoleColor(ERROR_COLOR);
        cout << ERROR_MARKER << "getClientDataByEmail - Exception: " << e.what() << endl;
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

                if (user_json_obj.contains("donations_list") && user_json_obj["donations_list"].is_array()) {
                    json client_donations_array_json = user_json_obj["donations_list"];
                    int num_donations_from_file = client_donations_array_json.size();

                    if (num_donations_from_file > 0) {
                        foundClient.donations = new (std::nothrow) donationn[num_donations_from_file];
                        if (foundClient.donations == nullptr) {
                            setConsoleColor(ERROR_COLOR);
                            cerr << ERROR_MARKER << "Memory allocation failed for loading client donations for " << email << endl;
                            setConsoleColor(DEFAULT_COLOR);
                            foundClient.number_ofdonations = 0;
                        } else {
                            foundClient.number_ofdonations = 0;
                            for (int i = 0; i < num_donations_from_file; ++i) {
                                const auto& d_json = client_donations_array_json[i];
                                if (d_json.is_object()) {
                                    foundClient.donations[foundClient.number_ofdonations].donation_id = d_json.value("donation_id", "");
                                    foundClient.donations[foundClient.number_ofdonations].charity_id = d_json.value("charity_id", 0);
                                    foundClient.donations[foundClient.number_ofdonations].amount = d_json.value("amount", 0.0f);
                                    foundClient.donations[foundClient.number_ofdonations].message = d_json.value("message", "");
                                    if (d_json.contains("d") && d_json["d"].is_object()) {
                                        foundClient.donations[foundClient.number_ofdonations].d.date = d_json["d"].value("date", "0000-00-00");
                                        foundClient.donations[foundClient.number_ofdonations].d.time = d_json["d"].value("time", "00:00:00");
                                    } else {
                                        foundClient.donations[foundClient.number_ofdonations].d.date = "0000-00-00";
                                        foundClient.donations[foundClient.number_ofdonations].d.time = "00:00:00";
                                    }
                                    foundClient.number_ofdonations++;
                                }
                            }
                        }
                    } else {
                        foundClient.number_ofdonations = 0;
                        foundClient.donations = nullptr;
                    }
                } else {
                     int legacy_donation_count = user_json_obj.value("number_ofdonations", 0);
                     if (legacy_donation_count > 0) {
                        setConsoleColor(WARNING_COLOR);
                        cout << "[WARN] User " << email << " has number_ofdonations > 0 but no 'donations_list' array. Past donations not loaded into client object for this session." << endl;
                        setConsoleColor(DEFAULT_COLOR);
                     }
                     foundClient.number_ofdonations = 0;
                     foundClient.donations = nullptr;
                }
            } catch (const json::type_error& e) {
                setConsoleColor(WARNING_COLOR);
                cout << WARNING_MARKER << "Type error processing user data for " << email << ": " << e.what() << endl;
                setConsoleColor(DEFAULT_COLOR);
                foundClient.user_id = -1;
                if (foundClient.donations != nullptr) { delete[] foundClient.donations; foundClient.donations = nullptr; }
                foundClient.number_ofdonations = 0;
            }
            break;
        }
    }
    return foundClient;
}

client registerNewUser() {
    client newUser;
    string plain_password_input;
    json users_json_array = json::array();
    auto now = chrono::system_clock::now();
    auto in_time_t = chrono::system_clock::to_time_t(now);
    tm time_info_tm;
#if defined(_MSC_VER) || defined(__MINGW32__) || defined(__MINGW64__)
    localtime_s(&time_info_tm, &in_time_t);
#else
    tm* p_time_info_tm = localtime(&in_time_t);
    if (p_time_info_tm) {
        time_info_tm = *p_time_info_tm;
    } else {
        time_info_tm = {};
    }
#endif
    ostringstream time_ss;
    time_ss << put_time(&time_info_tm, "%I:%M %p");
    string current_time_str = time_ss.str();
    if (current_time_str.rfind("0", 0) == 0 && current_time_str.length() > 1 && isdigit(current_time_str[1])) {
         current_time_str.erase(0, 1);
    }
    string registration_title = "U S E R   R E G I S T R A T I O N   A T   " + current_time_str;
    printStyledHeader(registration_title);

    setConsoleColor(PROMPT_COLOR); cout << PROMPT_ARROW << "Enter First Name: "; setConsoleColor(DEFAULT_COLOR); getline(cin, newUser.first_name);
    setConsoleColor(PROMPT_COLOR); cout << PROMPT_ARROW << "Enter Last Name: "; setConsoleColor(DEFAULT_COLOR); getline(cin, newUser.last_name);
    while (true) {
        setConsoleColor(PROMPT_COLOR); cout << PROMPT_ARROW << "Enter Email: "; setConsoleColor(DEFAULT_COLOR); getline(cin, newUser.email);
        if (isValidEmailFormat(newUser.email)) {
            client existingUser = getClientDataByEmail(newUser.email);
            if (existingUser.user_id == -1) {
                break;
            } else {
                setConsoleColor(ERROR_COLOR); cout << "  " << ERROR_MARKER << "This email address is already registered.\n" << "             Please use a different email or login.\n"; setConsoleColor(DEFAULT_COLOR);
            }
        } else {
            setConsoleColor(ERROR_COLOR); cout << "  " << ERROR_MARKER << "Invalid email format. (e.g., user@example.com).\n"; setConsoleColor(DEFAULT_COLOR);
        }
    }
    setConsoleColor(PROMPT_COLOR); cout << PROMPT_ARROW << "Enter Phone: "; setConsoleColor(DEFAULT_COLOR); getline(cin, newUser.phone);
    while(true){
        setConsoleColor(PROMPT_COLOR); cout << PROMPT_ARROW << "Enter password (min 8 characters): "; setConsoleColor(DEFAULT_COLOR); getline(cin, plain_password_input);
        if(plain_password_input.length() >= 8) {
            break;
        }
        setConsoleColor(ERROR_COLOR); cout << "  " << ERROR_MARKER << "Password too short. Please try again.\n"; setConsoleColor(DEFAULT_COLOR);
    }
    newUser.password = bcrypt::generateHash(plain_password_input);
    setConsoleColor(PROMPT_COLOR); cout << PROMPT_ARROW << "Enter Role (user/admin - default is 'user'): "; setConsoleColor(DEFAULT_COLOR); getline(cin, newUser.role);
    if (newUser.role != "admin" && newUser.role != "user") {
        setConsoleColor(INFO_COLOR); cout << "  " << INFO_MARKER << "Invalid role. Defaulting to 'user'.\n"; setConsoleColor(DEFAULT_COLOR);
        newUser.role = "user";
    }
    cout << LINE_SEPARATOR_THIN << endl;

    ifstream inputFile(USERS_JSON_FILE_PATH);
    if (inputFile.is_open()) {
        try {
            inputFile >> users_json_array;
            if (!users_json_array.is_array()) {
                 setConsoleColor(WARNING_COLOR); cout << WARNING_MARKER << "users.json is not JSON array. Starting fresh." << endl; setConsoleColor(DEFAULT_COLOR);
                 users_json_array = json::array();
            }
        } catch (const json::parse_error&) {
            setConsoleColor(WARNING_COLOR); cout << WARNING_MARKER << "Error parsing users.json. Will create/overwrite." << endl; setConsoleColor(DEFAULT_COLOR);
            users_json_array = json::array();
        }
        inputFile.close();
    } else {
        setConsoleColor(INFO_COLOR); cout << INFO_MARKER << "users.json not found. New file will be created." << endl; setConsoleColor(DEFAULT_COLOR);
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
    newUser.donations = nullptr;

    json user_json_to_add = {
        {"user_id", newUser.user_id},
        {"first_name", newUser.first_name},
        {"last_name", newUser.last_name},
        {"email", newUser.email},
        {"phone", newUser.phone},
        {"password", newUser.password},
        {"role", newUser.role},
        {"number_ofdonations", newUser.number_ofdonations},
        {"donations_list", json::array()}
    };
    users_json_array.push_back(user_json_to_add);

    ofstream outputFile(USERS_JSON_FILE_PATH);
    if (!outputFile.is_open()) {
        setConsoleColor(CRITICAL_COLOR); cout << CRITICAL_MARKER << "Could not open '" << USERS_JSON_FILE_PATH << "' for writing! User not saved.\n"; setConsoleColor(DEFAULT_COLOR);
        newUser.user_id = -1;
    } else {
        outputFile << setw(4) << users_json_array << endl;
        if (outputFile.fail()) {
             setConsoleColor(CRITICAL_COLOR); cout << CRITICAL_MARKER << "Error writing to '" << USERS_JSON_FILE_PATH << "'. User not saved.\n"; setConsoleColor(DEFAULT_COLOR);
             newUser.user_id = -1;
        } else {
            setConsoleColor(SUCCESS_COLOR); cout << LINE_SEPARATOR_THICK << endl; cout << SUCCESS_MARKER << "User '" << newUser.email << "' registered! ID: " << newUser.user_id << "\n"; cout << LINE_SEPARATOR_THICK << endl; setConsoleColor(DEFAULT_COLOR);
        }
        outputFile.close();
    }
    return newUser;
}

bool verifyUserLogin(const string& email, const string& plain_password_input) {
    ifstream inputFile(USERS_JSON_FILE_PATH);
    if (!inputFile.is_open()) { return false; }
    json users_json_data;
    try {
        inputFile >> users_json_data;
        if (!users_json_data.is_array()) { inputFile.close(); return false; }
    } catch (const std::exception&) { inputFile.close(); return false; }
    inputFile.close();
    for (const auto& user_obj : users_json_data) {
        if (user_obj.is_object() && user_obj.contains("email") && user_obj["email"].is_string() &&
            user_obj.contains("password") && user_obj["password"].is_string()) {
             if (user_obj.value("email","") == email) {
                 string stored_hashed_password = user_obj.value("password","");
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
    ifstream inputFile(USERS_JSON_FILE_PATH);
    if (inputFile.is_open()) {
        try {
            inputFile >> users_json_array;
            if (!users_json_array.is_array()) {
                setConsoleColor(WARNING_COLOR); cout << WARNING_MARKER << "users.json not valid array. Save aborted for: " << userToSave.email << "." << endl; setConsoleColor(DEFAULT_COLOR);
                inputFile.close(); return;
            }
        } catch (const json::parse_error& e) {
            setConsoleColor(WARNING_COLOR); cout << WARNING_MARKER << "Error parsing users.json: " << e.what() << ". Save aborted for: " << userToSave.email << endl; setConsoleColor(DEFAULT_COLOR);
            inputFile.close(); return;
        }
        inputFile.close();
    } else {
        setConsoleColor(INFO_COLOR); cout << INFO_MARKER << "users.json not found for save, cannot update: " << userToSave.email << endl; setConsoleColor(DEFAULT_COLOR);
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

            json client_donations_array_json = json::array();
            if (userToSave.donations != nullptr && userToSave.number_ofdonations > 0) {
                for (int i = 0; i < userToSave.number_ofdonations; ++i) {
                    const donationn& d = userToSave.donations[i];
                    json donation_entry_json;
                    donation_entry_json["donation_id"] = d.donation_id;
                    donation_entry_json["charity_id"] = d.charity_id;
                    donation_entry_json["amount"] = d.amount;
                    donation_entry_json["message"] = d.message;
                    donation_entry_json["d"]["date"] = d.d.date;
                    donation_entry_json["d"]["time"] = d.d.time;
                    client_donations_array_json.push_back(donation_entry_json);
                }
            }
            user_json_obj["donations_list"] = client_donations_array_json;
            userFoundAndUpdated = true;
            break;
        }
    }

    if (!userFoundAndUpdated) {
        setConsoleColor(ERROR_COLOR); cout << ERROR_MARKER << "User ID " << userToSave.user_id << " (Email: " << userToSave.email << ") NOT FOUND during save. Data NOT saved." << endl; setConsoleColor(DEFAULT_COLOR);
        return;
    }

    ofstream outputFile(USERS_JSON_FILE_PATH);
    if (!outputFile.is_open()) {
        setConsoleColor(CRITICAL_COLOR); cout << CRITICAL_MARKER << "Could not open '" << USERS_JSON_FILE_PATH << "' for writing! User data for " << userToSave.email << " not saved.\n"; setConsoleColor(DEFAULT_COLOR);
        return;
    }
    outputFile << setw(4) << users_json_array << endl;
    if (outputFile.fail()) {
        setConsoleColor(CRITICAL_COLOR); cout << CRITICAL_MARKER << "Error writing user data to '" << USERS_JSON_FILE_PATH << "' for " << userToSave.email << ".\n"; setConsoleColor(DEFAULT_COLOR);
    } else {
        setConsoleColor(INFO_COLOR); cout << INFO_MARKER << "User data for " << userToSave.email << " (ID: " << userToSave.user_id << ") saved successfully (including " << userToSave.number_ofdonations << " donations)." << endl; setConsoleColor(DEFAULT_COLOR);
    }
    outputFile.close();
}

bool checkIfAdmin(const string& email) {
    client user = getClientDataByEmail(email);
    return user.user_id != -1 && user.role == "admin";
}