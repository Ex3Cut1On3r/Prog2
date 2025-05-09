#include "structsandfunctions/functions.h"
#include "nlohman/json.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <limits>
#include <algorithm>

using namespace std;
using json = nlohmann::json;

void loadCharities(charity*& charities, int& charityCount) {
    string filePath = "../Project_ALL/data/charities.json";
    ifstream inputFile(filePath);
    json charities_json_data;

    delete[] charities;
    charities = nullptr;
    charityCount = 0;

    if (!inputFile.is_open()) {
        cerr << "⚠️ Could not open '" << filePath << "' for reading. Starting with no charities.\n";
        return;
    }
    try {
        inputFile >> charities_json_data;
        inputFile.close();
        if (!charities_json_data.is_array()) {
             cerr << "❌ Charity data file ('" << filePath << "') is not a JSON array! Starting with no charities.\n";
             return;
         }
        charityCount = charities_json_data.size();
        if (charityCount > 0) {
            charities = new (std::nothrow) charity[charityCount];
            if (charities == nullptr) {
                 cerr << "❌ Memory allocation failed while loading charities!\n";
                 charityCount = 0;
                 return;
            }
            for (int i = 0; i < charityCount; ++i) {
                const auto& charity_obj_json = charities_json_data[i];
                try {
                    if (charity_obj_json.is_object()) {
                        charities[i].charity_id = charity_obj_json.value("charity_id", -1);
                        charities[i].name = charity_obj_json.value("name", "Unknown");
                        charities[i].description = charity_obj_json.value("description", "No description");
                        charities[i].target_amount = charity_obj_json.value("target_amount", 0.0);
                        charities[i].current_amount = charity_obj_json.value("current_amount", 0.0);
                        charities[i].status = charity_obj_json.value("status", "Unknown");
                        if (charities[i].charity_id == -1 || charities[i].name == "Unknown") {
                             cerr << "⚠️ Warning: Charity at index " << i << " in JSON has missing ID or name.\n";
                        }
                    } else {
                        cerr << "⚠️ Warning: Item at index " << i << " in charities.json is not a valid charity object.\n";
                        charities[i].charity_id = -1; charities[i].name = "Invalid Entry";
                    }
                } catch (const json::type_error& e) {
                    cerr << "⚠️ Type error reading charity data at JSON index " << i << ": " << e.what() << endl;
                    charities[i].charity_id = -1; charities[i].name = "Error - Corrupted Data";
                }
            }
             cout << "[Info] Loaded " << charityCount << " charities from " << filePath << "." << endl;
        } else {
             cout << "[Info] " << filePath << " is empty or no valid charities found. Starting with no charities." << endl;
        }
    } catch (const json::parse_error& e) {
        cerr << "❌ Error parsing JSON from charities file ('" << filePath << "'): " << e.what() << ". Starting with no charities.\n";
        delete[] charities; charities = nullptr; charityCount = 0;
    } catch (const std::bad_alloc& e) {
         cerr << "❌ Memory allocation failed (bad_alloc) while loading charities: " << e.what() << endl;
         delete[] charities; charities = nullptr; charityCount = 0;
    } catch (const std::exception& e) {
        cerr << "❌ An unexpected error occurred while loading charities: " << e.what() << endl;
        delete[] charities; charities = nullptr; charityCount = 0;
    }
}

void saveCharities(const charity* charities, int charityCount) {
    string filePath = "../Project_ALL/data/charities.json";
    ofstream outputFile(filePath);
    if (!outputFile.is_open()) {
        cerr << "❌ Could not open '" << filePath << "' for writing! Changes not saved.\n";
        return;
    }
    json charities_json_array = json::array();
    if (charities != nullptr) {
        for (int i = 0; i < charityCount; ++i) {
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
        cerr << "❌ Error occurred while writing to '" << filePath << "'. Data might not be saved correctly." << endl;
    }
    outputFile.close();
    if (!outputFile.good() && outputFile.is_open()){
         cerr << "❌ Error occurred while closing '" << filePath << "'. Data might not be saved correctly." << endl;
    } else {
        if (charityCount > 0 || (charities == nullptr && charityCount == 0) ) {
             cout << "[Info] Saved " << charityCount << " charities to " << filePath << "." << endl;
        }
    }
}

void addCharity(charity*& charities, int& charityCount) {
    charity newCharity;
    cout << "\nEnter charity name: ";
    getline(cin, newCharity.name);
    cout << "Enter charity description: ";
    getline(cin, newCharity.description);
    cout << "Enter charity target amount (non-negative): ";
    while (!(cin >> newCharity.target_amount) || newCharity.target_amount < 0) {
        cout << "Invalid input. Please enter a non-negative number: ";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
    cout << "Enter charity current amount (non-negative): ";
     while (!(cin >> newCharity.current_amount) || newCharity.current_amount < 0) {
        cout << "Invalid input. Please enter a non-negative number: ";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
    if (newCharity.current_amount > newCharity.target_amount && newCharity.target_amount > 0) {
        cout << "Warning: Current amount (" << newCharity.current_amount
             << ") exceeds target amount (" << newCharity.target_amount << ").\n";
    }
    cout << "Enter charity status (e.g., Active, Pending, Completed): ";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    getline(cin, newCharity.status);
    int max_id = 0;
    if (charities != nullptr) {
        for(int i = 0; i < charityCount; ++i) {
            if (charities[i].charity_id > max_id) {
                max_id = charities[i].charity_id;
            }
        }
    }
    newCharity.charity_id = max_id + 1;
    charity* temp = new (std::nothrow) charity[charityCount + 1];
    if (temp == nullptr) {
         cerr << "❌ Memory allocation failed when adding charity!\n";
         return;
    }
    if (charities != nullptr) {
        for (int i = 0; i < charityCount; ++i) {
            temp[i] = charities[i];
        }
    }
    temp[charityCount] = newCharity;
    delete[] charities;
    charities = temp;
    charityCount++;
    cout << "✅ Charity '" << newCharity.name << "' added successfully! ID: " << newCharity.charity_id << "\n";
}

void modifyCharity(charity* charities, int charityCount) {
    if (charityCount == 0) {
        cout << "No charities available to modify.\n";
        return;
    }
    int charityID;
    cout << "\nEnter the ID of the charity to modify: ";
    while (!(cin >> charityID)) {
        cout << "Invalid input. Please enter a number: ";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    bool found = false;
    for (int i = 0; i < charityCount; ++i) {
        if (charities[i].charity_id == charityID) {
            found = true;
            cout << "Modifying charity: " << charities[i].name << " (ID: " << charities[i].charity_id << ")\n";
            string input;
            cout << "Current Name: " << charities[i].name << ". Enter new name (or press Enter to keep): ";
            getline(cin, input); if (!input.empty()) charities[i].name = input;
            cout << "Current Description: " << charities[i].description << ". Enter new description (or press Enter to keep): ";
            getline(cin, input); if (!input.empty()) charities[i].description = input;
            cout << "Current Target Amount: $" << fixed << setprecision(2) << charities[i].target_amount << ". Enter new target amount: $";
            double newTarget; string targetInputStr; getline(cin, targetInputStr);
            if (!targetInputStr.empty()) {
                try { newTarget = stod(targetInputStr);
                    if (newTarget >= 0) charities[i].target_amount = newTarget;
                    else cout << "Target amount must be non-negative. Keeping old value.\n";
                } catch (const std::exception&) { cout << "Invalid number for target. Keeping old value.\n"; }
            }
            cout << "Current Amount: $" << fixed << setprecision(2) << charities[i].current_amount << ". Enter new current amount: $";
            double newCurrent; string currentInputStr; getline(cin, currentInputStr);
            if(!currentInputStr.empty()){
                try { newCurrent = stod(currentInputStr);
                    if (newCurrent >= 0) charities[i].current_amount = newCurrent;
                    else cout << "Current amount must be non-negative. Keeping old value.\n";
                } catch (const std::exception&) { cout << "Invalid number for current. Keeping old value.\n"; }
            }
            cout << "Current Status: " << charities[i].status << ". Enter new status (or press Enter to keep): ";
            getline(cin, input); if (!input.empty()) charities[i].status = input;
            cout << "✅ Charity modified successfully!\n";
            break;
        }
    }
    if (!found) cout << "❌ Charity with ID " << charityID << " not found.\n";
}

void removeCharity(charity*& charities, int& charityCount) {
    if (charityCount == 0) {
        cout << "No charities available to remove.\n";
        return;
    }
    int charityID;
    cout << "\nEnter the ID of the charity to remove: ";
    while (!(cin >> charityID)) {
        cout << "Invalid input. Please enter a number: ";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
    bool found = false; int indexToRemove = -1;
    for (int i = 0; i < charityCount; ++i) {
        if (charities[i].charity_id == charityID) {
            found = true; indexToRemove = i;
            cout << "Found charity: " << charities[i].name << " (ID: " << charities[i].charity_id
                 << "). Are you sure you want to remove it? (y/n): ";
            char confirm;
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cin.get(confirm);
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            if (tolower(confirm) != 'y') {
                cout << "Removal cancelled.\n"; return;
            }
            break;
        }
    }
    if (found) {
        if (charityCount == 1) {
            delete[] charities; charities = nullptr; charityCount = 0;
        } else {
            charity* temp = new (std::nothrow) charity[charityCount - 1];
            if (temp == nullptr) {
                cerr << "❌ Memory allocation failed when removing charity!\n"; return;
            }
            for (int i = 0, j = 0; i < charityCount; ++i) {
                if (i == indexToRemove) continue;
                temp[j++] = charities[i];
            }
            delete[] charities; charities = temp; charityCount--;
        }
        cout << "✅ Charity removed successfully!\n";
    } else {
        cout << "❌ Charity with ID " << charityID << " not found.\n";
    }
}

void displayCharities(const charity* charities, int charityCount) {
    if (charityCount == 0) {
        cout << "\nNo charities available to display.\n";
        return;
    }

    const int id_width = 6;
    const int name_width = 22;
    const int desc_width = 35;
    const int amount_width = 13;
    const int status_width = 15;

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

    cout << "\n" << border_line << "\n";

    string title = "--- List of Charities ---";
    int available_width_for_title = border_line.length() - 2;
    int title_len = title.length();
    int padding_total = available_width_for_title - title_len;
    int padding_left = 0;
    int padding_right = 0;

    if (padding_total > 0) {
        padding_left = padding_total / 2;
        padding_right = padding_total - padding_left;
    }

    cout << "|" << string(padding_left, ' ') << title << string(padding_right, ' ') << "|\n";
    cout << border_line << "\n";

    cout << "| " << left << setw(id_width) << "ID" << " | "
         << left << setw(name_width) << "Name" << " | "
         << left << setw(desc_width) << "Description" << " | "
         << right << setw(amount_width) << "Current ($)" << " | "
         << right << setw(amount_width) << "Target ($)" << " | "
         << left << setw(status_width) << "Status" << " |\n";

    cout << border_line << "\n";

    for (int i = 0; i < charityCount; ++i) {
        string displayName = charities[i].name;
        const string ellipsis = "...";
        int ellipsis_len = ellipsis.length();

        if (displayName.length() > name_width) {
            if (name_width > ellipsis_len) {
                displayName = displayName.substr(0, name_width - ellipsis_len) + ellipsis;
            } else {
                displayName = ellipsis.substr(0, name_width);
            }
        }

        string displayDesc = charities[i].description;
        if (displayDesc.length() > desc_width) {
            if (desc_width > ellipsis_len) {
                displayDesc = displayDesc.substr(0, desc_width - ellipsis_len) + ellipsis;
            } else {
                displayDesc = ellipsis.substr(0, desc_width);
            }
        }

        string displayStatus = charities[i].status;
        if (displayStatus.length() > status_width) {
             if (status_width > ellipsis_len) {
                displayStatus = displayStatus.substr(0, status_width - ellipsis_len) + ellipsis;
            } else {
                displayStatus = ellipsis.substr(0, status_width);
            }
        }

        cout << "| " << left << setw(id_width) << charities[i].charity_id << " | "
             << left << setw(name_width) << displayName << " | "
             << left << setw(desc_width) << displayDesc << " | "
             << right << setw(amount_width) << fixed << setprecision(2) << charities[i].current_amount << " | "
             << right << setw(amount_width) << fixed << setprecision(2) << charities[i].target_amount << " | "
             << left << setw(status_width) << displayStatus << " |\n";
    }

    cout << border_line << "\n";
}