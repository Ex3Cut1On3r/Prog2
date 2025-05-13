#include "structsandfunctions/functions.h"
#include "nlohman/json.hpp"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <limits>
#include <string>
#include <vector>
#include <ctime>
#include <sstream>
#include <chrono>

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
using std::to_string;
using json = nlohmann::json;

// extern datee getCurrentDateTime(); // Ensure this is available

json loadDonationsFile(const string& filePath) {
    ifstream inFile(filePath);
    json donationsArray = json::array();
    if (inFile.is_open()) {
        try {
            inFile >> donationsArray;
            if (!donationsArray.is_array()) {
                cout << "[Warning] " << filePath << " does not contain a JSON array. Initializing as empty." << endl;
                donationsArray = json::array();
            }
        } catch (json::parse_error& e) {
            cerr << "[Error] Parsing " << filePath << ": " << e.what() << ". Initializing as empty." << endl;
            donationsArray = json::array();
        }
        inFile.close();
    }
    return donationsArray;
}

void saveDonationsFile(const string& filePath, const json& donationsArray) {
    ofstream outFile(filePath);
    if (outFile.is_open()) {
        outFile << setw(4) << donationsArray << endl;
        outFile.close();
    } else {
        cerr << "❌ Error: Could not open " << filePath << " for writing. Donations not saved to global file." << endl;
    }
}

void appendDonationToGlobalFile(const donationn& clientDonationRecord, const client& donatingUser, const charity& targetCharity) {
    string filePath = "../Project_ALL/data/donations.json";
    json allDonations = loadDonationsFile(filePath);

    json donationEntryJson;
    donationEntryJson["donation_id"] = clientDonationRecord.donation_id;
    donationEntryJson["charity_id"] = clientDonationRecord.charity_id;
    donationEntryJson["amount"] = clientDonationRecord.amount; // This is from donationn
    donationEntryJson["d"]["date"] = clientDonationRecord.d.date;
    donationEntryJson["d"]["time"] = clientDonationRecord.d.time;
    donationEntryJson["message"] = clientDonationRecord.message;

    donationEntryJson["user_id"] = donatingUser.user_id;
    donationEntryJson["user_name"] = donatingUser.first_name + " " + donatingUser.last_name;
    donationEntryJson["charity_name"] = targetCharity.name;

    std::tm t{};
    std::istringstream ss(clientDonationRecord.d.date + " " + clientDonationRecord.d.time);
    ss >> std::get_time(&t, "%Y-%m-%d %H:%M:%S");
    long long unix_timestamp = 0;
    if (!ss.fail()) {
        std::time_t time_since_epoch = std::mktime(&t);
        if (time_since_epoch != -1) {
            unix_timestamp = static_cast<long long>(time_since_epoch);
        } else {
            unix_timestamp = static_cast<long long>(std::time(nullptr)); // Fallback
        }
    } else {
        unix_timestamp = static_cast<long long>(std::time(nullptr)); // Fallback
    }
    donationEntryJson["donation_timestamp"] = unix_timestamp;
    donationEntryJson["donation_amount"] = clientDonationRecord.amount; // Explicitly ensure donation_amount is top-level

    allDonations.push_back(donationEntryJson);
    saveDonationsFile(filePath, allDonations);
}

void makeDonation(client& user, charity*& charities, int charityCount, bool& successFlag) {
    successFlag = false;
    if (charityCount == 0) {
        cout << "No charities available to donate to." << endl;
        return;
    }
    displayCharities(charities, charityCount);
    int charityIdChoice;
    cout << "Enter the ID of the charity you want to donate to (0 to cancel): ";
    while (!(cin >> charityIdChoice) || charityIdChoice < 0 ) {
        cout << "Invalid input. Please enter a non-negative charity ID (0 to cancel): ";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    if (charityIdChoice == 0) {
        cout << "Donation cancelled." << endl;
        return;
    }
    int charityIndex = -1;
    for (int i = 0; i < charityCount; ++i) { if (charities[i].charity_id == charityIdChoice) { charityIndex = i; break; }}
    if (charityIndex == -1) {
        cout << "Invalid charity ID." << endl;
        return;
    }
    if (!charities[charityIndex].status) {
        cout << "This charity ('" << charities[charityIndex].name
             << "') is currently CLOSED for donations. Thank you for your interest!" << endl;
        return;
    }
    float donationAmount;
    float amount_left_to_target = 0.0f;
    bool hasFiniteTarget = (charities[charityIndex].target_amount > 0.0f);
    if (hasFiniteTarget) {
        amount_left_to_target = charities[charityIndex].target_amount - charities[charityIndex].current_amount;
        if (amount_left_to_target <= 0.0f) {
            if (charities[charityIndex].status) {
                charities[charityIndex].status = false;
            }
            cout << "This charity ('" << charities[charityIndex].name
                 << "') has just reached its target and is now CLOSED. Thank you for your interest!" << endl;
            return;
        }
    }
    cout << "Donating to: " << charities[charityIndex].name << endl;
    cout << "Target Amount: $" << fixed << setprecision(2) << charities[charityIndex].target_amount << endl;
    cout << "Currently Raised: $" << fixed << setprecision(2) << charities[charityIndex].current_amount << endl;
    if (hasFiniteTarget) {
        cout << "Amount needed to reach target: $" << fixed << setprecision(2) << amount_left_to_target << endl;
    }
    while (true) {
        cout << "Enter donation amount: $"; string inputAmountStr; getline(cin, inputAmountStr);
        if (inputAmountStr.empty()){ cout << "No amount entered. Please enter a numeric amount." << endl; continue; }
        try { donationAmount = stof(inputAmountStr); }
        catch (const std::invalid_argument&) { cout << "Invalid input. Numeric amount required. Please try again." << endl; continue; }
        catch (const std::out_of_range& ) { cout << "Amount out of range. Please try again." << endl; continue; }
        if (donationAmount <= 0) {
            cout << "Donation amount must be positive. Please try again." << endl;
        } else if (hasFiniteTarget && donationAmount > amount_left_to_target) {
            cout << "Your donation of $" << fixed << setprecision(2) << donationAmount
                 << " exceeds the remaining amount needed ($" << fixed << setprecision(2) << amount_left_to_target << ")." << endl;
            cout << "Please enter an amount up to $" << fixed << setprecision(2) << amount_left_to_target << "." << endl;
        } else {
            break;
        }
    }

    donationn newDonationToStoreInClient;
    newDonationToStoreInClient.donation_id = "D_Cl" + to_string(time(0)) + "_" + to_string(user.user_id) + "_" + to_string(user.number_ofdonations);
    newDonationToStoreInClient.charity_id = charities[charityIndex].charity_id;
    newDonationToStoreInClient.amount = donationAmount;
    newDonationToStoreInClient.d = getCurrentDateTime();
    cout << "Optional: Enter a message for your donation (press Enter for no message): ";
    getline(cin, newDonationToStoreInClient.message);

    int new_num_donations_for_client = user.number_ofdonations + 1;
    donationn* temp_client_donations = new (std::nothrow) donationn[new_num_donations_for_client];
    if (temp_client_donations == nullptr) {
        cerr << "❌ Memory allocation failed for storing donation in client data. Donation not processed." << endl;
        return;
    }
    for (int i = 0; i < user.number_ofdonations; ++i) {
        temp_client_donations[i] = user.donations[i];
    }
    temp_client_donations[user.number_ofdonations] = newDonationToStoreInClient;
    if (user.donations != nullptr) {
        delete[] user.donations;
    }
    user.donations = temp_client_donations;

    charities[charityIndex].current_amount += donationAmount;
    charities[charityIndex].d = getCurrentDateTime();
    user.number_ofdonations++;

    cout << "✅ Thank you, " << user.first_name << "! Your donation of $" << fixed << setprecision(2) << donationAmount
         << " to '" << charities[charityIndex].name << "' has been processed." << endl;

    appendDonationToGlobalFile(newDonationToStoreInClient, user, charities[charityIndex]);

    if (hasFiniteTarget && charities[charityIndex].current_amount >= charities[charityIndex].target_amount) {
        if (charities[charityIndex].status) {
            charities[charityIndex].status = false;
            cout << "---FUNDRAISING GOAL REACHED - CHARITY CLOSED---" << endl;
            cout << "Congratulations! '" << charities[charityIndex].name
                 << "' has now reached its fundraising target and is CLOSED for further donations." << endl;
        }
    }
    successFlag = true;
}

void displayUserDonations(const client& user, const charity* allCharities, int totalCharityCount) {
    if (user.number_ofdonations == 0 || user.donations == nullptr) {
        cout << "\nYou have not made any donations yet (based on current session data).\n";
        return;
    }
    cout << "\n--- Your Donation History (Current Session) ---" << endl;
    cout << left << setw(5) << "No."
         << setw(20) << "Donation ID"
         << setw(25) << "Charity Name"
         << right << setw(15) << "Amount ($)"
         << "  " << left << setw(12) << "Date"
         << setw(10) << "Time"
         << "  Message" << endl;
    cout << string(100, '-') << endl;
    for (int i = 0; i < user.number_ofdonations; ++i) {
        const donationn& d = user.donations[i];
        string charityName = "Unknown/Deleted";
        for(int j=0; j < totalCharityCount; ++j) {
            if (allCharities[j].charity_id == d.charity_id) {
                charityName = allCharities[j].name;
                break;
            }
        }
        string displayDonId = d.donation_id;
        if (displayDonId.length() > 18) displayDonId = displayDonId.substr(0, 15) + "...";
        string displayCharityName = charityName;
        if (displayCharityName.length() > 23) displayCharityName = displayCharityName.substr(0, 20) + "...";
        string displayMessage = d.message;
        if (displayMessage.length() > 20) displayMessage = displayMessage.substr(0, 17) + "...";

        cout << left << setw(5) << (i + 1)
             << setw(20) << displayDonId
             << setw(25) << displayCharityName
             << right << setw(15) << fixed << setprecision(2) << d.amount
             << "  " << left << setw(12) << d.d.date
             << setw(10) << d.d.time;
        if (!d.message.empty()) {
            cout << "  Msg: " << displayMessage;
        }
        cout << endl;
    }
    cout << string(100, '-') << endl;
}

void modifyDonation(client& user, charity*& charities, int charityCount, const charity* allCharities, int totalCharityCount) {
    if (user.number_ofdonations == 0 || user.donations == nullptr) {
        cout << "You have no donations to modify.\n";
        return;
    }
    displayUserDonations(user, allCharities, totalCharityCount);
    if (user.number_ofdonations == 0) return;
    cout << "Enter the No. of the donation you wish to modify (from the list above, 0 to cancel): ";
    int selection_num;
    while (!(cin >> selection_num) || selection_num < 0 || selection_num > user.number_ofdonations) {
        cout << "Invalid input. Please enter a valid number from the list (1 to "
             << user.number_ofdonations << ") or 0 to cancel: ";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    if (selection_num == 0) {
        cout << "Modification cancelled." << endl;
        return;
    }
    int donationIndexToModify = selection_num - 1;
    donationn& originalDonationInClient = user.donations[donationIndexToModify];
    float oldAmount = originalDonationInClient.amount;
    int charityIdOfOriginalDonation = originalDonationInClient.charity_id;
    string originalDonationId = originalDonationInClient.donation_id;

    string charityNameOfDonation = "Unknown";
    int originalCharityIndex = -1;
    for(int i=0; i < charityCount; ++i) {
        if (charities[i].charity_id == charityIdOfOriginalDonation) {
            charityNameOfDonation = charities[i].name;
            originalCharityIndex = i;
            break;
        }
    }

    cout << "Modifying your donation No. " << selection_num << " of $" << fixed << setprecision(2) << oldAmount
         << " to '" << charityNameOfDonation << "' (Donation ID: " << originalDonationId << ")." << endl;

    float newDonationAmount;
    float max_new_amount_for_this_donation = -1.0f;
    bool charity_target_limit_applies = false;

    if (originalCharityIndex != -1 && charities[originalCharityIndex].target_amount > 0.0f) {
        charity_target_limit_applies = true;
        if (charities[originalCharityIndex].status) {
            max_new_amount_for_this_donation = (charities[originalCharityIndex].target_amount - charities[originalCharityIndex].current_amount) + oldAmount;
             cout << "Charity '" << charities[originalCharityIndex].name << "' is Active. Max new amount for this donation: $" << fixed << setprecision(2) << max_new_amount_for_this_donation << endl;
        } else {
             cout << "Charity '" << charities[originalCharityIndex].name << "' is CLOSED. You can only decrease this donation or keep it the same (max $" << fixed << setprecision(2) << oldAmount << ")." << endl;
             max_new_amount_for_this_donation = oldAmount;
        }
    }

    while (true) {
        cout << "Enter new donation amount: $"; string inputAmountStr; getline(cin, inputAmountStr);
        if (inputAmountStr.empty()){ cout << "No amount entered. Please try again." << endl; continue; }
        try { newDonationAmount = stof(inputAmountStr); }
        catch (const std::exception&) { cout << "Invalid number. Please try again." << endl; continue; }

        if (newDonationAmount <= 0) {
            cout << "New donation amount must be positive." << endl;
        } else if (charity_target_limit_applies && newDonationAmount > max_new_amount_for_this_donation) {
            cout << "New amount $" << fixed << setprecision(2) << newDonationAmount
                 << " is too high. Max allowed new amount for this donation is $"
                 << fixed << setprecision(2) << max_new_amount_for_this_donation << "." << endl;
        } else {
            break;
        }
    }

    float amountDifference = newDonationAmount - oldAmount;

    originalDonationInClient.amount = newDonationAmount;
    originalDonationInClient.d = getCurrentDateTime();
    cout << "Optional: Enter new message for this donation (press Enter to keep old: '" << originalDonationInClient.message << "'): ";
    string newMessage;
    getline(cin, newMessage);
    if (!newMessage.empty()){
        originalDonationInClient.message = newMessage;
    }

    string filePath = "../Project_ALL/data/donations.json";
    json allDonations = loadDonationsFile(filePath);
    bool foundInJson = false;
    charity modifiedCharityDetails; // To pass to appendDonationToGlobalFile if we re-add
    bool reAddNeeded = false;

    for(auto it = allDonations.begin(); it != allDonations.end(); ++it) {
        if(it->value("donation_id", "") == originalDonationId) {
            // Update the existing entry in the global log
            (*it)["amount"] = newDonationAmount;
            (*it)["message"] = originalDonationInClient.message;
            (*it)["d"]["date"] = originalDonationInClient.d.date;
            (*it)["d"]["time"] = originalDonationInClient.d.time;
            (*it)["donation_timestamp"] = static_cast<long long>(std::time(nullptr)); // Update timestamp to now
            // user_name and charity_name likely remain the same for a modification
            // charity_id should also remain the same

            foundInJson = true;
            break;
        }
    }
    if (foundInJson) {
        saveDonationsFile(filePath, allDonations);
    } else {
        cout << "⚠️ Warning: Donation ID " << originalDonationId << " not found in global donations.json to modify. Logging as new entry if charity details found." << endl;
        // If not found, we might want to log the modification as a new entry if the original was somehow lost from global.
        // This would require finding the charity again. For simplicity now, we just warn.
        // Or, re-add it:
        if (originalCharityIndex != -1) {
             appendDonationToGlobalFile(originalDonationInClient, user, charities[originalCharityIndex]);
             cout << "Logged modified donation as a new entry in global donations.json due to original not being found." << endl;
        }
    }


    if (originalCharityIndex != -1) {
        charities[originalCharityIndex].current_amount += amountDifference;
        charities[originalCharityIndex].d = getCurrentDateTime();
        bool oldCharityStatusBool = charities[originalCharityIndex].status;
        charities[originalCharityIndex].status = !(charities[originalCharityIndex].target_amount > 0.0f &&
                                                charities[originalCharityIndex].current_amount >= charities[originalCharityIndex].target_amount);
        if (oldCharityStatusBool != charities[originalCharityIndex].status) {
            cout << "Charity '" << charities[originalCharityIndex].name << "' status changed to: "
                 << (charities[originalCharityIndex].status ? "Active" : "Closed") << endl;
        }
        cout << "✅ Charity '" << charities[originalCharityIndex].name << "' current amount updated to $"
             << fixed << setprecision(2) << charities[originalCharityIndex].current_amount << endl;
    } else if (charityIdOfOriginalDonation != -1) {
        cout << "⚠️ Warning: Original charity (ID: " << charityIdOfOriginalDonation
             << ") for this donation was not found in the current charities list. Its total amount cannot be updated." << endl;
    }
    cout << "✅ Donation No. " << selection_num << " successfully modified in your record.\n";
}

void cancelDonation(client& user, charity*& charities, int charityCount, const charity* allCharities, int totalCharityCount) {
    if (user.number_ofdonations == 0 || user.donations == nullptr) {
        cout << "You have no donations to cancel.\n";
        return;
    }
    displayUserDonations(user, allCharities, totalCharityCount);
    if (user.number_ofdonations == 0) return;
    cout << "Enter the No. of the donation you wish to cancel (from the list above, 0 to cancel): ";
    int selection_num;
    while (!(cin >> selection_num) || selection_num < 0 || selection_num > user.number_ofdonations) {
        cout << "Invalid input. Please enter a valid number (1 to "
             << user.number_ofdonations << ") or 0 to cancel: ";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    if (selection_num == 0) {
        cout << "Cancellation cancelled." << endl;
        return;
    }
    int indexToCancel = selection_num - 1;
    donationn donationToCancelDetails = user.donations[indexToCancel];
    float cancelledAmount = donationToCancelDetails.amount;
    int charityIdOfCancelledDonation = donationToCancelDetails.charity_id;
    string donationIdCancelled = donationToCancelDetails.donation_id;

    cout << "Cancelling your donation No. " << selection_num << " of $" << fixed << setprecision(2) << cancelledAmount
         << " (Donation ID: " << donationIdCancelled << ")." << endl;

    if (user.number_ofdonations == 1) {
        delete[] user.donations;
        user.donations = nullptr;
    } else {
        donationn* temp_donations = new (std::nothrow) donationn[user.number_ofdonations - 1];
        if (temp_donations == nullptr) {
            cerr << "❌ Memory allocation failed during donation cancellation. Operation aborted." << endl;
            return;
        }
        for (int i = 0, j = 0; i < user.number_ofdonations; ++i) {
            if (i == indexToCancel) {
                continue;
            }
            temp_donations[j++] = user.donations[i];
        }
        delete[] user.donations;
        user.donations = temp_donations;
    }
    user.number_ofdonations--;

    string filePath = "../Project_ALL/data/donations.json";
    json currentGlobalDonations = loadDonationsFile(filePath);
    json updatedGlobalDonations = json::array();
    bool foundInJson = false;
    for(const auto& jsonDonation : currentGlobalDonations) {
        if(jsonDonation.value("donation_id", "") == donationIdCancelled) {
            foundInJson = true;
        } else {
            updatedGlobalDonations.push_back(jsonDonation);
        }
    }
    if (foundInJson) {
        saveDonationsFile(filePath, updatedGlobalDonations);
        cout << "Donation record removed from global donations.json log." << endl;
    } else {
        cout << "⚠️ Warning: Donation ID " << donationIdCancelled << " not found in donations.json. Global log not updated for cancellation." << endl;
    }

    int originalCharityIndex = -1;
    for (int i = 0; i < charityCount; ++i) { if (charities[i].charity_id == charityIdOfCancelledDonation) { originalCharityIndex = i; break; }}
    if (originalCharityIndex != -1) {
        charities[originalCharityIndex].current_amount -= cancelledAmount;
        if (charities[originalCharityIndex].current_amount < 0) {
            charities[originalCharityIndex].current_amount = 0.0f;
        }
        charities[originalCharityIndex].d = getCurrentDateTime();
        bool oldCharityStatusBool = charities[originalCharityIndex].status;
        charities[originalCharityIndex].status = !(charities[originalCharityIndex].target_amount > 0.0f &&
                                                charities[originalCharityIndex].current_amount >= charities[originalCharityIndex].target_amount);
        if (oldCharityStatusBool != charities[originalCharityIndex].status) {
            cout << "Charity '" << charities[originalCharityIndex].name << "' status changed to: "
                 << (charities[originalCharityIndex].status ? "Active" : "Closed") << endl;
        }
        cout << "✅ Charity '" << charities[originalCharityIndex].name << "' current amount updated to $"
             << fixed << setprecision(2) << charities[originalCharityIndex].current_amount << endl;
    } else if (charityIdOfCancelledDonation != -1) {
        cout << "⚠️ Warning: Original charity (ID: " << charityIdOfCancelledDonation
             << ") for the cancelled donation was not found. Its total amount cannot be updated." << endl;
    }
    cout << "✅ Donation successfully cancelled from your record.\n";
}