#include "structsandfunctions/functions.h" // Includes all necessary declarations from your central header
#include "nlohman/json.hpp"
#include <iostream>
#include <fstream>
#include <iomanip>    // For setw, fixed, setprecision
#include <limits>     // For numeric_limits
#include <string>     // For std::string operations
#include <ctime>      // For time(0)
// No <vector> needed for this version as we are manually managing JSON array iteration

using namespace std;
using json = nlohmann::json;

// Forward declaration for the helper if it's defined later in this file
void writeAllDonationsToFile(const json& donationsArray);

void writeDonationToFile(const client& user, const charity& charityDetails, double donationAmount, time_t donationTimestamp) {
    string filePath = "../Project_ALL/data/donations.json";
    json donationsArray = json::array();

    ifstream inFile(filePath);
    if (inFile.is_open()) {
        try {
            inFile >> donationsArray;
            if (!donationsArray.is_array()) {
                cerr << "⚠️ " << filePath << " does not contain a JSON array. Initializing as empty." << endl;
                donationsArray = json::array();
            }
        } catch (json::parse_error& e) {
            cerr << "⚠️ Error parsing " << filePath << ": " << e.what()
                 << ". Initializing as empty." << endl;
            donationsArray = json::array();
        }
        inFile.close();
    } else {
        cout << "[Info] " << filePath << " not found. A new file will be created." << endl;
    }

    json newDonationEntry;
    newDonationEntry["user_id"] = user.user_id;
    newDonationEntry["user_name"] = user.first_name + " " + user.last_name;
    newDonationEntry["charity_id"] = charityDetails.charity_id;
    newDonationEntry["charity_name"] = charityDetails.name;
    newDonationEntry["donation_amount"] = donationAmount;
    newDonationEntry["donation_timestamp"] = donationTimestamp;

    donationsArray.push_back(newDonationEntry);
    writeAllDonationsToFile(donationsArray);
}

void makeDonation(client& user, charity*& charities, int& charityCount, bool& donationMadeSuccessfully) {
    donationMadeSuccessfully = false;
    if (charityCount == 0) {
        cout << "No charities available to donate to.\n";
        return;
    }
    int charityID; double donationAmount;
    cout << "\nEnter charity ID to donate to: ";
    while (!(cin >> charityID) || charityID <= 0) {
        cout << "Invalid input. Please enter a positive charity ID: ";
        cin.clear(); cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
    bool found = false;
    int charityIndex = -1;
    for (int i = 0; i < charityCount; ++i) {
        if (charities[i].charity_id == charityID) {
            found = true;
            charityIndex = i;
            break;
        }
    }

    if (!found) {
        cout << "❌ Charity with ID " << charityID << " not found.\n";
        return;
    }

    cout << "Donating to: " << charities[charityIndex].name << " (Current: $" << fixed << setprecision(2) << charities[charityIndex].current_amount
         << ", Target: $" << fixed << setprecision(2) << charities[charityIndex].target_amount << ")\n";
    if (charities[charityIndex].status != "Active") {
        cout << "Sorry, charity '" << charities[charityIndex].name << "' is not Active (Status: "
             << charities[charityIndex].status << ").\n";
        return;
    }
    while (true) {
        cout << "Enter donation amount: $";
        if (!(cin >> donationAmount)) {
            cout << "Invalid input. Numeric amount required.\n";
            cin.clear(); cin.ignore(numeric_limits<streamsize>::max(), '\n'); continue;
        }
        if (donationAmount <= 0) {
            cout << "Donation amount must be positive.\n";
            cin.clear(); cin.ignore(numeric_limits<streamsize>::max(), '\n');
        } else {
            charities[charityIndex].current_amount += donationAmount;
            user.number_ofdonations++;
            cout << "✅ Thank you for your $" << fixed << setprecision(2) << donationAmount
                 << " donation to " << charities[charityIndex].name << "!\n";
            cout << charities[charityIndex].name << " new current amount: $" << fixed << setprecision(2) << charities[charityIndex].current_amount << endl;
            donationMadeSuccessfully = true;

            time_t current_time = time(0);
            writeDonationToFile(user, charities[charityIndex], donationAmount, current_time);
            break;
        }
    }
}

void displayUserDonations(const client& user) {
    if (user.number_ofdonations == 0) {
        cout << "\nYou have not made any donations yet.\n";
        return;
    }

    cout << "\n--- Your Donations Summary ---\n";
    cout << "Total individual donations made: " << user.number_ofdonations << endl;

    string filePath = "../Project_ALL/data/donations.json";
    ifstream inFile(filePath);
    json donationsArray;

    if(inFile.is_open()){
        try {
            inFile >> donationsArray;
            if(donationsArray.is_array() && !donationsArray.empty()){
                cout << "--- Your Donation History (Raw Timestamps) ---" << endl;
                bool foundDonationsForUser = false;
                int displayIndex = 1;

                const int num_width = 5;
                const int charity_name_width = 25;
                const int amount_width = 15;
                const int timestamp_width = 20; // Assuming timestamp is just a number

                cout << left << setw(num_width) << "No."
                        << setw(charity_name_width) << "Charity Name"
                        << right << setw(amount_width) << "Amount ($)"
                        << "  " << left << setw(timestamp_width) << "Timestamp" << endl;
                cout << string(num_width + charity_name_width + amount_width + timestamp_width + 6, '-') << endl; // Adjusted line width

                for(const auto& donation : donationsArray){
                    if(donation.value("user_id", -1) == user.user_id){
                        if(!foundDonationsForUser) {
                            foundDonationsForUser = true;
                        }
                        string charityName = donation.value("charity_name", "N/A");
                        if (charityName.length() >= charity_name_width) {
                            charityName = charityName.substr(0, charity_name_width - 1) + "…";
                        }

                        cout << left << setw(num_width) << displayIndex++
                                << setw(charity_name_width) << charityName
                                << right << setw(amount_width) << fixed << setprecision(2) << donation.value("donation_amount", 0.0)
                                << "  " << left << setw(timestamp_width) << donation.value("donation_timestamp", (long long)0) << endl;
                    }
                }
                if(foundDonationsForUser) {
                        cout << string(num_width + charity_name_width + amount_width + timestamp_width + 6, '-') << endl;
                } else {
                    cout << "No detailed donation records found for you in donations.json (though count is > 0)." << endl;
                }
            } else if (donationsArray.is_array() && donationsArray.empty() && user.number_ofdonations > 0) {
                cout << "donations.json is empty, but your donation count is " << user.number_ofdonations
                     << ". Records might be missing or system is out of sync." << endl;
            } else {
                 cout << "No donation records in donations.json." << endl;
            }
        } catch (json::parse_error& e) {
            cerr << "⚠️ Error parsing donations.json for display: " << e.what() << endl;
        }
        inFile.close();
    } else {
        cout << "Could not open donations.json to display history." << endl;
    }
}


void modifyDonation(client& user, charity*& charities, int& charityCount) {
    if (user.number_ofdonations == 0) {
        cout << "You have no donations to modify.\n";
        return;
    }

    displayUserDonations(user);
    if (user.number_ofdonations == 0) return; // In case displayUserDonations showed no records for this user from file

    cout << "Enter the No. of the donation you wish to modify (from the list above): ";
    int selection_num;
    while (!(cin >> selection_num) || selection_num <= 0) {
        cout << "Invalid input. Please enter a valid number from the list: ";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }

    string filePath = "../Project_ALL/data/donations.json";
    json donationsArray;
    ifstream inFile(filePath);
    if (!inFile.is_open()) {
        cerr << "❌ Could not open donations.json to modify donation.\n";
        return;
    }
    try {
        inFile >> donationsArray;
        if (!donationsArray.is_array()) {
            cerr << "❌ donations.json is not a valid array. Cannot modify donation.\n";
            inFile.close();
            return;
        }
    } catch (json::parse_error& e) {
        cerr << "❌ Error parsing donations.json: " << e.what() << endl;
        inFile.close();
        return;
    }
    inFile.close();

    int currentUserDonationIndex = 0;
    int targetDonationJsonIndex = -1;

    for (int i = 0; i < donationsArray.size(); ++i) {
        const auto& donation = donationsArray[i];
        if (donation.value("user_id", -1) == user.user_id) {
            currentUserDonationIndex++;
            if (currentUserDonationIndex == selection_num) {
                targetDonationJsonIndex = i;
                break;
            }
        }
    }

    if (targetDonationJsonIndex == -1) {
        cout << "❌ Donation selection No. " << selection_num << " not found for your account.\n";
        return;
    }

    json& donationToModify = donationsArray[targetDonationJsonIndex];
    double oldAmount = donationToModify.value("donation_amount", 0.0);
    int    charityIdOfDonation = donationToModify.value("charity_id", -1);
    string charityNameOfDonation = donationToModify.value("charity_name", "Unknown");

    cout << "Modifying your donation of $" << fixed << setprecision(2) << oldAmount
         << " to '" << charityNameOfDonation << "'." << endl;

    double newDonationAmount;
    cout << "Enter new donation amount: $";
    while (!(cin >> newDonationAmount) || newDonationAmount <= 0) {
        cout << "Invalid input. Please enter a positive number: ";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }

    double amountDifference = newDonationAmount - oldAmount;

    donationToModify["donation_amount"] = newDonationAmount;
    donationToModify["modified_timestamp"] = time(0); // Add a modification timestamp

    writeAllDonationsToFile(donationsArray);

    bool charityInMemoryFound = false;
    for (int i = 0; i < charityCount; ++i) {
        if (charities[i].charity_id == charityIdOfDonation) {
            charities[i].current_amount += amountDifference;
            cout << "✅ Charity '" << charities[i].name << "' current amount updated to $"
                 << fixed << setprecision(2) << charities[i].current_amount << endl;
            charityInMemoryFound = true;
            break;
        }
    }
    if (!charityInMemoryFound && charityIdOfDonation != -1) {
        cerr << "⚠️ Could not find charity with ID " << charityIdOfDonation
             << " in memory to update its amount. donations.json is updated, but charities array might be stale." << endl;
    }

    cout << "✅ Donation amount successfully modified to $" << fixed << setprecision(2) << newDonationAmount << ".\n";
}


void cancelDonation(client& user, charity*& charities, int& charityCount) {
    if (user.number_ofdonations == 0) {
        cout << "You have no donations to cancel.\n";
        return;
    }

    displayUserDonations(user);
    if (user.number_ofdonations == 0) return;

    cout << "Enter the No. of the donation you wish to cancel (from the list above): ";
    int selection_num;
    while (!(cin >> selection_num) || selection_num <= 0) {
        cout << "Invalid input. Please enter a valid number from the list: ";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }

    string filePath = "../Project_ALL/data/donations.json";
    json donationsArray;
    ifstream inFile(filePath);

    if (!inFile.is_open()) {
        cerr << "❌ Could not open donations.json to cancel donation.\n";
        return;
    }
    try {
        inFile >> donationsArray;
        if (!donationsArray.is_array()) {
            cerr << "❌ donations.json is not a valid array. Cannot cancel donation.\n";
            inFile.close();
            return;
        }
    } catch (json::parse_error& e) {
        cerr << "❌ Error parsing donations.json: " << e.what() << endl;
        inFile.close();
        return;
    }
    inFile.close();

    int currentUserDonationIndex = 0;
    int targetDonationJsonIndex = -1;
    json newDonationsArray = json::array();
    double cancelledAmount = 0;
    int charityIdOfCancelledDonation = -1;

    for (int i = 0; i < donationsArray.size(); ++i) {
        const auto& donation = donationsArray[i];
        if (donation.value("user_id", -1) == user.user_id) {
            currentUserDonationIndex++;
            if (currentUserDonationIndex == selection_num) {
                targetDonationJsonIndex = i;
                cancelledAmount = donation.value("donation_amount", 0.0);
                charityIdOfCancelledDonation = donation.value("charity_id", -1);
                cout << "Cancelling your donation of $" << fixed << setprecision(2) << cancelledAmount
                     << " to '" << donation.value("charity_name", "Unknown") << "'." << endl;
            } else {
                newDonationsArray.push_back(donation);
            }
        } else {
            newDonationsArray.push_back(donation);
        }
    }

    if (targetDonationJsonIndex == -1) {
        cout << "❌ Donation selection No. " << selection_num << " not found for your account.\n";
        return;
    }

    writeAllDonationsToFile(newDonationsArray);

    bool charityInMemoryFound = false;
    if (charityIdOfCancelledDonation != -1) {
        for (int i = 0; i < charityCount; ++i) {
            if (charities[i].charity_id == charityIdOfCancelledDonation) {
                charities[i].current_amount -= cancelledAmount;
                if (charities[i].current_amount < 0) charities[i].current_amount = 0;
                cout << "✅ Charity '" << charities[i].name << "' current amount updated to $"
                     << fixed << setprecision(2) << charities[i].current_amount << endl;
                charityInMemoryFound = true;
                break;
            }
        }
    }
     if (!charityInMemoryFound && charityIdOfCancelledDonation != -1) {
        cerr << "⚠️ Could not find charity with ID " << charityIdOfCancelledDonation
             << " in memory to update its amount after cancellation." << endl;
    }

    user.number_ofdonations--;
    cout << "✅ Donation successfully cancelled.\n";
}

void writeAllDonationsToFile(const json& donationsArray) {
    string filePath = "../Project_ALL/data/donations.json";
    ofstream outFile(filePath);
    if (outFile.is_open()) {
        outFile << setw(4) << donationsArray << endl;
        outFile.close();
    } else {
        cerr << "❌ Could not open " << filePath << " for writing. Donation records not saved." << endl;
    }
}