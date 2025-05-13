#include "structsandfunctions/functions.h"
#include "nlohman/json.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <limits>
#include <algorithm> // Still useful for other things, but not strictly for tolower loop
#include <string>
#include <cctype>    // For ::tolower

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#endif
//colors
#ifdef _WIN32
const WORD CC_DEFAULT = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
const WORD CC_TABLE_BORDER = FOREGROUND_BLUE | FOREGROUND_GREEN;
const WORD CC_TABLE_HEADER = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY;
const WORD CC_TABLE_TEXT = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
const WORD CC_INFO = FOREGROUND_BLUE | FOREGROUND_INTENSITY;
const WORD CC_WARNING = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY;
const WORD CC_ERROR = FOREGROUND_RED | FOREGROUND_INTENSITY;
const WORD CC_SUCCESS = FOREGROUND_GREEN | FOREGROUND_INTENSITY;
const WORD CC_STATUS_ACTIVE = FOREGROUND_GREEN | FOREGROUND_INTENSITY;
const WORD CC_STATUS_CLOSED = FOREGROUND_RED | FOREGROUND_INTENSITY;
void setDisplayCharitiesConsoleColor(WORD color) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, color);
}
#else
const WORD CC_DEFAULT = 0; const WORD CC_TABLE_BORDER = 0; const WORD CC_TABLE_HEADER = 0;
const WORD CC_TABLE_TEXT = 0; const WORD CC_INFO = 0; const WORD CC_WARNING = 0;
const WORD CC_ERROR = 0; const WORD CC_SUCCESS = 0;
const WORD CC_STATUS_ACTIVE = 0; const WORD CC_STATUS_CLOSED = 0;
void setDisplayCharitiesConsoleColor(WORD color) { (void)color; }
#endif


using std::cout;
using std::cerr;
using std::endl;
using std::string;
using std::left;
using std::right;
using std::setw;
using std::fixed;
using std::setprecision;
using std::ifstream;
using std::ofstream;
using std::cin;
using std::getline;
using std::numeric_limits;
using std::streamsize;
using json = nlohmann::json;


void displayCharities(const charity* charities, int charityCount) {
    if (charityCount == 0) {
        setDisplayCharitiesConsoleColor(CC_INFO);
        cout << "\nNo charities available to display.\n";
        setDisplayCharitiesConsoleColor(CC_DEFAULT);
        return;
    }
    const int id_width = 6;
    const int name_width = 22;
    const int desc_width = 35;
    const int amount_width = 13;
    const int status_width = 12;
    string border_segment_id = string(id_width + 2, '-');
    string border_segment_name = string(name_width + 2, '-');
    string border_segment_desc = string(desc_width + 2, '-');
    string border_segment_amount = string(amount_width + 2, '-');
    string border_segment_status = string(status_width + 2, '-');
    string border_line = "+";
    border_line += border_segment_id + "+";
    border_line += border_segment_name + "+";
    border_line += border_segment_desc + "+";
    border_line += border_segment_amount + "+";
    border_line += border_segment_amount + "+";
    border_line += border_segment_status + "+";
    setDisplayCharitiesConsoleColor(CC_TABLE_BORDER);
    cout << "\n" << border_line << "\n";
    string title = "--- List of Charities ---";
    int available_width_for_title = border_line.length() - 2;
    int title_len = title.length();
    int padding_total = available_width_for_title - title_len;
    int padding_left = (padding_total > 0) ? padding_total / 2 : 0;
    int padding_right = (padding_total > 0) ? padding_total - padding_left : 0;
    cout << "|";
    setDisplayCharitiesConsoleColor(CC_TABLE_HEADER);
    cout << string(padding_left, ' ') << title << string(padding_right, ' ');
    setDisplayCharitiesConsoleColor(CC_TABLE_BORDER);
    cout << "|\n";
    cout << border_line << "\n";
    cout << "| ";
    setDisplayCharitiesConsoleColor(CC_TABLE_HEADER); cout << left << setw(id_width) << "ID";
    setDisplayCharitiesConsoleColor(CC_TABLE_BORDER); cout << " | ";
    setDisplayCharitiesConsoleColor(CC_TABLE_HEADER); cout << left << setw(name_width) << "Name";
    setDisplayCharitiesConsoleColor(CC_TABLE_BORDER); cout << " | ";
    setDisplayCharitiesConsoleColor(CC_TABLE_HEADER); cout << left << setw(desc_width) << "Description";
    setDisplayCharitiesConsoleColor(CC_TABLE_BORDER); cout << " | ";
    setDisplayCharitiesConsoleColor(CC_TABLE_HEADER); cout << right << setw(amount_width) << "Raised ($)";
    setDisplayCharitiesConsoleColor(CC_TABLE_BORDER); cout << " | ";
    setDisplayCharitiesConsoleColor(CC_TABLE_HEADER); cout << right << setw(amount_width) << "Target ($)";
    setDisplayCharitiesConsoleColor(CC_TABLE_BORDER); cout << " | ";
    setDisplayCharitiesConsoleColor(CC_TABLE_HEADER); cout << left << setw(status_width) << "Status";
    setDisplayCharitiesConsoleColor(CC_TABLE_BORDER); cout << " |\n";
    cout << border_line << "\n";
    setDisplayCharitiesConsoleColor(CC_DEFAULT);
    for (int i = 0; i < charityCount; ++i) {
        if (charities[i].charity_id == -1 && (charities[i].name.find("Invalid") != string::npos || charities[i].name.find("Corrupted") != string::npos)) continue;
        string displayName = charities[i].name; const string ellipsis = "..."; int ellipsis_len = ellipsis.length();
        if (displayName.length() > name_width) displayName = (name_width > ellipsis_len) ? displayName.substr(0, name_width - ellipsis_len) + ellipsis : ellipsis.substr(0, name_width);
        string displayDesc = charities[i].description;
        if (displayDesc.length() > desc_width) displayDesc = (desc_width > ellipsis_len) ? displayDesc.substr(0, desc_width - ellipsis_len) + ellipsis : ellipsis.substr(0, desc_width);
        setDisplayCharitiesConsoleColor(CC_TABLE_BORDER); cout << "| ";
        setDisplayCharitiesConsoleColor(CC_TABLE_TEXT); cout << left << setw(id_width) << charities[i].charity_id;
        setDisplayCharitiesConsoleColor(CC_TABLE_BORDER); cout << " | ";
        setDisplayCharitiesConsoleColor(CC_TABLE_TEXT); cout << left << setw(name_width) << displayName;
        setDisplayCharitiesConsoleColor(CC_TABLE_BORDER); cout << " | ";
        setDisplayCharitiesConsoleColor(CC_TABLE_TEXT); cout << left << setw(desc_width) << displayDesc;
        setDisplayCharitiesConsoleColor(CC_TABLE_BORDER); cout << " | ";
        setDisplayCharitiesConsoleColor(CC_TABLE_TEXT); cout << right << setw(amount_width) << fixed << setprecision(2) << charities[i].current_amount;
        setDisplayCharitiesConsoleColor(CC_TABLE_BORDER); cout << " | ";
        setDisplayCharitiesConsoleColor(CC_TABLE_TEXT); cout << right << setw(amount_width) << fixed << setprecision(2) << charities[i].target_amount;
        setDisplayCharitiesConsoleColor(CC_TABLE_BORDER); cout << " | ";
        string statusText = charities[i].status ? "Active" : "Closed";
        WORD statusColorToUse = charities[i].status ? CC_STATUS_ACTIVE : CC_STATUS_CLOSED;
        setDisplayCharitiesConsoleColor(statusColorToUse);
        cout << left << setw(status_width) << statusText;
        setDisplayCharitiesConsoleColor(CC_TABLE_BORDER);
        cout << " |\n";
        setDisplayCharitiesConsoleColor(CC_DEFAULT);
    }
    setDisplayCharitiesConsoleColor(CC_TABLE_BORDER);
    cout << border_line << "\n";
    setDisplayCharitiesConsoleColor(CC_DEFAULT);
}

void loadCharities(charity*& charities, int& charityCount) {
    string filePath = "../Project_ALL/data/charities.json";
    ifstream inputFile(filePath);
    json charities_json_data;

    if (charities != nullptr) {
        delete[] charities;
        charities = nullptr;
    }
    charityCount = 0;

    if (!inputFile.is_open()) {
        setDisplayCharitiesConsoleColor(CC_WARNING);
        cerr << "⚠️  Could not open '" << filePath << "' for reading. Starting with no charities.\n";
        setDisplayCharitiesConsoleColor(CC_DEFAULT);
        return;
    }

    try {
        inputFile >> charities_json_data;
        inputFile.close();

        if (!charities_json_data.is_array()) {
             setDisplayCharitiesConsoleColor(CC_ERROR);
             cerr << "❌ Charity data file ('" << filePath << "') is not a JSON array! Starting with no charities.\n";
             setDisplayCharitiesConsoleColor(CC_DEFAULT);
             return;
         }

        charityCount = charities_json_data.size();
        if (charityCount > 0) {
            charities = new (std::nothrow) charity[charityCount];
            if (charities == nullptr) {
                 setDisplayCharitiesConsoleColor(CC_ERROR);
                 cerr << "❌ Memory allocation failed while loading charities!\n";
                 setDisplayCharitiesConsoleColor(CC_DEFAULT);
                 charityCount = 0;
                 return;
            }

            int validCharitiesLoaded = 0;
            for (int i = 0; i < charityCount; ++i) {
                const auto& charity_obj_json = charities_json_data[i];
                try {
                    if (charity_obj_json.is_object()) {
                        charities[i].charity_id = charity_obj_json.value("charity_id", -1);
                        charities[i].name = charity_obj_json.value("name", "Unknown Name");
                        charities[i].description = charity_obj_json.value("description", "No description.");
                        charities[i].target_amount = charity_obj_json.value("target_amount", 0.0f);
                        charities[i].current_amount = charity_obj_json.value("current_amount", 0.0f);

                        if (charity_obj_json.contains("status") && charity_obj_json["status"].is_boolean()) {
                            charities[i].status = charity_obj_json["status"].get<bool>();
                        } else if (charity_obj_json.contains("status") && charity_obj_json["status"].is_string()) {
                            string statusStr = charity_obj_json["status"].get<string>();
                            // Simple loop for tolower
                            for (size_t j = 0; j < statusStr.length(); ++j) {
                                statusStr[j] = ::tolower(static_cast<unsigned char>(statusStr[j]));
                            }
                            charities[i].status = (statusStr == "open" || statusStr == "active");
                        } else {
                            charities[i].status = !(charities[i].target_amount > 0.0f && charities[i].current_amount >= charities[i].target_amount);
                        }

                        if (charities[i].charity_id == -1 || charities[i].name == "Unknown Name") {
                             setDisplayCharitiesConsoleColor(CC_WARNING);
                             cerr << "⚠️  Warning: Charity at JSON index " << i << " has missing ID or name. Status: " << (charities[i].status ? "Active" : "Closed") << "\n";
                             setDisplayCharitiesConsoleColor(CC_DEFAULT);
                        }
                        validCharitiesLoaded++;
                    } else {
                        setDisplayCharitiesConsoleColor(CC_WARNING);
                        cerr << "⚠️  Warning: Item at index " << i << " in charities.json is not a valid charity object.\n";
                        setDisplayCharitiesConsoleColor(CC_DEFAULT);
                        charities[i].charity_id = -1; charities[i].name = "Invalid JSON Entry"; charities[i].status = false;
                    }
                } catch (const json::type_error& e) {
                    setDisplayCharitiesConsoleColor(CC_ERROR);
                    cerr << "❌ Type error reading charity data at JSON index " << i << ": " << e.what() << endl;
                    setDisplayCharitiesConsoleColor(CC_DEFAULT);
                    charities[i].charity_id = -1; charities[i].name = "Error - Corrupted Data"; charities[i].status = false;
                }
            }

            if (validCharitiesLoaded > 0) {
                setDisplayCharitiesConsoleColor(CC_INFO);
                cout << "[Info] Loaded " << validCharitiesLoaded << " of " << charityCount << " charities from " << filePath << "." << endl;
                setDisplayCharitiesConsoleColor(CC_DEFAULT);
            } else if (charityCount > 0 && validCharitiesLoaded == 0){
                 setDisplayCharitiesConsoleColor(CC_WARNING);
                 cout << "[Info] " << filePath << " contained entries, but none were valid charities. Starting with no charities." << endl;
                 setDisplayCharitiesConsoleColor(CC_DEFAULT);
                 delete[] charities; charities = nullptr; charityCount = 0;
            }
        } else {
             setDisplayCharitiesConsoleColor(CC_INFO);
             cout << "[Info] " << filePath << " is empty or contains no charity data. Starting with no charities." << endl;
             setDisplayCharitiesConsoleColor(CC_DEFAULT);
        }
    } catch (const json::parse_error& e) {
        setDisplayCharitiesConsoleColor(CC_ERROR);
        cerr << "❌ Error parsing JSON from charities file ('" << filePath << "'): " << e.what() << ". Starting with no charities.\n";
        setDisplayCharitiesConsoleColor(CC_DEFAULT);
        if (charities != nullptr) { delete[] charities; charities = nullptr; }
        charityCount = 0;
    } catch (const std::bad_alloc& e) {
         setDisplayCharitiesConsoleColor(CC_ERROR);
         cerr << "❌ Memory allocation failed (bad_alloc) while loading charities: " << e.what() << endl;
         setDisplayCharitiesConsoleColor(CC_DEFAULT);
         if (charities != nullptr) { delete[] charities; charities = nullptr; }
         charityCount = 0;
    } catch (const std::exception& e) {
        setDisplayCharitiesConsoleColor(CC_ERROR);
        cerr << "❌ An unexpected error occurred while loading charities: " << e.what() << endl;
        setDisplayCharitiesConsoleColor(CC_DEFAULT);
        if (charities != nullptr) { delete[] charities; charities = nullptr; }
        charityCount = 0;
    }
}



void saveCharities(const charity* charities, int charityCount) {
    string filePath = "../Project_ALL/data/charities.json";
    ofstream outputFile(filePath);
    if (!outputFile.is_open()) {
        setDisplayCharitiesConsoleColor(CC_ERROR);
        cerr << "❌ Could not open '" << filePath << "' for writing! Charity changes not saved.\n";
        setDisplayCharitiesConsoleColor(CC_DEFAULT);
        return;
    }
    json charities_json_array = json::array();
    if (charities != nullptr) {
        for (int i = 0; i < charityCount; ++i) {
            if (charities[i].charity_id == -1 && charities[i].name.find("Invalid") != string::npos) continue;
            json charity_obj = {
                {"charity_id", charities[i].charity_id},
                {"name", charities[i].name},
                {"description", charities[i].description},
                {"target_amount", charities[i].target_amount},
                {"current_amount", charities[i].current_amount},
                {"status", charities[i].status}
            };
            charities_json_array.push_back(charity_obj);
        }
    }
    outputFile << setw(4) << charities_json_array << endl;
    if (outputFile.fail()) {
        setDisplayCharitiesConsoleColor(CC_ERROR);
        cerr << "❌ Error occurred while writing charity data to '" << filePath << "'. Data might not be saved correctly." << endl;
        setDisplayCharitiesConsoleColor(CC_DEFAULT);
    }
    outputFile.close();
    if (!outputFile.good() && outputFile.is_open()){
         setDisplayCharitiesConsoleColor(CC_ERROR);
         cerr << "❌ Error occurred while closing '" << filePath << "'. Data might not be saved correctly." << endl;
         setDisplayCharitiesConsoleColor(CC_DEFAULT);
    } else if (!outputFile.fail()) {
        setDisplayCharitiesConsoleColor(CC_INFO);
        cout << "[Info] Saved " << charities_json_array.size() << " charities to " << filePath << "." << endl;
        setDisplayCharitiesConsoleColor(CC_DEFAULT);
    }
}

void addCharity(charity*& charities, int& charityCount) {
    charity newCharity;
    setDisplayCharitiesConsoleColor(CC_INFO); cout << "\n--- Add New Charity ---" << endl; setDisplayCharitiesConsoleColor(CC_DEFAULT);
    cout << "Enter charity name: "; getline(cin, newCharity.name);
    while (newCharity.name.empty()) { setDisplayCharitiesConsoleColor(CC_WARNING); cout << "Charity name cannot be empty. Please enter charity name: "; setDisplayCharitiesConsoleColor(CC_DEFAULT); getline(cin, newCharity.name); }
    cout << "Enter charity description: "; getline(cin, newCharity.description);
    cout << "Enter charity target amount (e.g., 1000.00, or 0 if no specific target): "; string targetInputStr;
    while (true) { getline(cin, targetInputStr); try { newCharity.target_amount = stof(targetInputStr); if (newCharity.target_amount < 0) { setDisplayCharitiesConsoleColor(CC_WARNING); cout << "Target amount cannot be negative. Please enter a non-negative value (or 0): "; setDisplayCharitiesConsoleColor(CC_DEFAULT); } else break; } catch (const std::invalid_argument&) { setDisplayCharitiesConsoleColor(CC_WARNING); cout << "Invalid number format. Please enter a numeric value (e.g., 1000.00): "; setDisplayCharitiesConsoleColor(CC_DEFAULT); } catch (const std::out_of_range&) { setDisplayCharitiesConsoleColor(CC_WARNING); cout << "Number out of range. Please enter a valid amount: "; setDisplayCharitiesConsoleColor(CC_DEFAULT); }}
    cout << "Enter charity current amount raised (must be <= target if target > 0): "; string currentInputStr;
    while (true) { getline(cin, currentInputStr); try { newCharity.current_amount = stof(currentInputStr); if (newCharity.current_amount < 0) { setDisplayCharitiesConsoleColor(CC_WARNING); cout << "Current amount cannot be negative. Please enter a non-negative value: "; setDisplayCharitiesConsoleColor(CC_DEFAULT); } else if (newCharity.target_amount > 0.0f && newCharity.current_amount > newCharity.target_amount) { setDisplayCharitiesConsoleColor(CC_WARNING); cout << "Current amount cannot exceed target amount ($" << fixed << setprecision(2) << newCharity.target_amount << "). Please enter a valid amount: "; setDisplayCharitiesConsoleColor(CC_DEFAULT); } else break; } catch (const std::invalid_argument&) { setDisplayCharitiesConsoleColor(CC_WARNING); cout << "Invalid number format. Please enter a numeric value: "; setDisplayCharitiesConsoleColor(CC_DEFAULT); } catch (const std::out_of_range&) { setDisplayCharitiesConsoleColor(CC_WARNING); cout << "Number out of range. Please enter a valid amount: "; setDisplayCharitiesConsoleColor(CC_DEFAULT); }}
    newCharity.status = !(newCharity.target_amount > 0.0f && newCharity.current_amount >= newCharity.target_amount);
    cout << "Status automatically set to: " << (newCharity.status ? "Active" : "Closed") << endl;
    int max_id = 0; if (charities != nullptr) { for(int i = 0; i < charityCount; ++i) { if (charities[i].charity_id > max_id) max_id = charities[i].charity_id; }}
    newCharity.charity_id = max_id + 1;
    newCharity.d = getCurrentDateTime(); // Make sure getCurrentDateTime() is available and returns datee
    charity* temp = new (std::nothrow) charity[charityCount + 1];
    if (temp == nullptr) { setDisplayCharitiesConsoleColor(CC_ERROR); cerr << "❌ Memory allocation failed when adding charity!\n"; setDisplayCharitiesConsoleColor(CC_DEFAULT); return; }
    if (charities != nullptr) { for (int i = 0; i < charityCount; ++i) temp[i] = charities[i]; delete[] charities; }
    temp[charityCount] = newCharity; charities = temp; charityCount++;
    setDisplayCharitiesConsoleColor(CC_SUCCESS); cout << "✅ Charity '" << newCharity.name << "' added successfully! ID: " << newCharity.charity_id << "\n"; setDisplayCharitiesConsoleColor(CC_DEFAULT);
}

void modifyCharity(charity*& charities, int charityCount) {
    if (charityCount == 0) { setDisplayCharitiesConsoleColor(CC_INFO); cout << "No charities available to modify.\n"; setDisplayCharitiesConsoleColor(CC_DEFAULT); return; }
    int charityID;
    displayCharities(charities, charityCount);
    cout << "Enter the ID of the charity to modify (0 to cancel): ";
    while (!(cin >> charityID) || charityID < 0 ) { setDisplayCharitiesConsoleColor(CC_WARNING); cout << "Invalid input. Please enter a number: "; setDisplayCharitiesConsoleColor(CC_DEFAULT); cin.clear(); cin.ignore(numeric_limits<streamsize>::max(), '\n'); }
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    if (charityID == 0) { setDisplayCharitiesConsoleColor(CC_INFO); cout << "Modification cancelled.\n"; setDisplayCharitiesConsoleColor(CC_DEFAULT); return; }
    int charityIndex = -1;
    for (int i = 0; i < charityCount; ++i) { if (charities[i].charity_id == charityID) { charityIndex = i; break; }}
    if (charityIndex == -1) { setDisplayCharitiesConsoleColor(CC_ERROR); cout << "❌ Charity with ID " << charityID << " not found.\n"; setDisplayCharitiesConsoleColor(CC_DEFAULT); return; }
    charity& charityToModify = charities[charityIndex];
    setDisplayCharitiesConsoleColor(CC_INFO); cout << "Modifying charity: " << charityToModify.name << " (ID: " << charityToModify.charity_id << ")\n"; setDisplayCharitiesConsoleColor(CC_DEFAULT);
    string input; float tempFloat;
    cout << "Current Name: " << charityToModify.name << ".\nEnter new name (or press Enter to keep): "; getline(cin, input); if (!input.empty()) charityToModify.name = input;
    cout << "Current Description: " << charityToModify.description << ".\nEnter new description (or press Enter to keep): "; getline(cin, input); if (!input.empty()) charityToModify.description = input;
    cout << "Current Target Amount: $" << fixed << setprecision(2) << charityToModify.target_amount << ".\nEnter new target amount (or press Enter to keep, e.g., 1500.00): $"; getline(cin, input);
    if (!input.empty()) { try { tempFloat = stof(input); if (tempFloat < 0) { setDisplayCharitiesConsoleColor(CC_WARNING); cout << "Target amount must be non-negative. Keeping old value: $" << charityToModify.target_amount << "\n"; setDisplayCharitiesConsoleColor(CC_DEFAULT); } else charityToModify.target_amount = tempFloat; } catch (const std::exception&) { setDisplayCharitiesConsoleColor(CC_WARNING); cout << "Invalid number format for target. Keeping old value: $" << charityToModify.target_amount << "\n"; setDisplayCharitiesConsoleColor(CC_DEFAULT); }}
    cout << "Current Amount Raised: $" << fixed << setprecision(2) << charityToModify.current_amount << ".\nEnter new amount raised (or press Enter to keep): $"; getline(cin, input);
    if (!input.empty()) { try { tempFloat = stof(input); if (tempFloat < 0) { setDisplayCharitiesConsoleColor(CC_WARNING); cout << "Current amount must be non-negative. Keeping old value: $" << charityToModify.current_amount << "\n"; setDisplayCharitiesConsoleColor(CC_DEFAULT); } else if (charityToModify.target_amount > 0.0f && tempFloat > charityToModify.target_amount) { setDisplayCharitiesConsoleColor(CC_WARNING); cout << "Current amount raised cannot exceed target amount ($" << fixed << setprecision(2) << charityToModify.target_amount << "). Keeping old value: $" << charityToModify.current_amount << "\n"; setDisplayCharitiesConsoleColor(CC_DEFAULT); } else charityToModify.current_amount = tempFloat; } catch (const std::exception&) { setDisplayCharitiesConsoleColor(CC_WARNING); cout << "Invalid number format for current amount. Keeping old value: $" << charityToModify.current_amount << "\n"; setDisplayCharitiesConsoleColor(CC_DEFAULT); }}
    bool oldStatusBool = charityToModify.status;
    charityToModify.status = !(charityToModify.target_amount > 0.0f && charityToModify.current_amount >= charityToModify.target_amount);
    if (oldStatusBool != charityToModify.status) {
        setDisplayCharitiesConsoleColor(CC_INFO);
        cout << "Status updated from '" << (oldStatusBool ? "Active" : "Closed") << "' to '" << (charityToModify.status ? "Active" : "Closed") << "' based on amounts.\n";
        setDisplayCharitiesConsoleColor(CC_DEFAULT);
    }
    charityToModify.d = getCurrentDateTime(); // Make sure getCurrentDateTime() is available
    setDisplayCharitiesConsoleColor(CC_SUCCESS); cout << "✅ Charity modified successfully!\n"; setDisplayCharitiesConsoleColor(CC_DEFAULT);
}

void removeCharity(charity*& charities, int& charityCount) {
    if (charityCount == 0) { setDisplayCharitiesConsoleColor(CC_INFO); cout << "No charities available to remove.\n"; setDisplayCharitiesConsoleColor(CC_DEFAULT); return; }
    int charityID;
    displayCharities(charities, charityCount);
    cout << "Enter the ID of the charity to remove (0 to cancel): ";
    while (!(cin >> charityID) || charityID < 0 ) { setDisplayCharitiesConsoleColor(CC_WARNING); cout << "Invalid input. Please enter a number: "; setDisplayCharitiesConsoleColor(CC_DEFAULT); cin.clear(); cin.ignore(numeric_limits<streamsize>::max(), '\n'); }
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    if (charityID == 0) { setDisplayCharitiesConsoleColor(CC_INFO); cout << "Removal cancelled.\n"; setDisplayCharitiesConsoleColor(CC_DEFAULT); return; }
    int indexToRemove = -1;
    for (int i = 0; i < charityCount; ++i) { if (charities[i].charity_id == charityID) { indexToRemove = i; break; }}
    if (indexToRemove != -1) {
        setDisplayCharitiesConsoleColor(CC_WARNING); cout << "Found charity: " << charities[indexToRemove].name << " (ID: " << charities[indexToRemove].charity_id << ").\nAre you sure you want to PERMANENTLY remove it? (y/n): "; setDisplayCharitiesConsoleColor(CC_DEFAULT);
        char confirm; cin.get(confirm); cin.ignore(numeric_limits<streamsize>::max(), '\n');
        if (tolower(confirm) != 'y') { setDisplayCharitiesConsoleColor(CC_INFO); cout << "Removal cancelled.\n"; setDisplayCharitiesConsoleColor(CC_DEFAULT); return; }
        if (charityCount == 1 && indexToRemove == 0) { delete[] charities; charities = nullptr; charityCount = 0; }
        else { charity* temp = new (std::nothrow) charity[charityCount - 1]; if (temp == nullptr) { setDisplayCharitiesConsoleColor(CC_ERROR); cerr << "❌ Memory allocation failed when removing charity!\n"; setDisplayCharitiesConsoleColor(CC_DEFAULT); return; } for (int i = 0, j = 0; i < charityCount; ++i) { if (i == indexToRemove) continue; temp[j++] = charities[i]; } delete[] charities; charities = temp; charityCount--; }
        setDisplayCharitiesConsoleColor(CC_SUCCESS); cout << "✅ Charity removed successfully!\n"; setDisplayCharitiesConsoleColor(CC_DEFAULT);
    } else { setDisplayCharitiesConsoleColor(CC_ERROR); cout << "❌ Charity with ID " << charityID << " not found.\n"; setDisplayCharitiesConsoleColor(CC_DEFAULT); }
}