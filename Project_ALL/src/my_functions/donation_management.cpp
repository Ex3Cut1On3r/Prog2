#include "structsandfunctions/functions.h"
#include "nlohman/json.hpp"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <limits>
#include <string>
#include <vector>
#include <ctime>

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
    donationn newDonation;
    newDonation.donation_id = "D" + to_string(time(0)) + "_" + to_string(user.user_id) + "_" + to_string(user.number_ofdonations);
    newDonation.charity_id = charities[charityIndex].charity_id;
    newDonation.amount = donationAmount;
    newDonation.d = getCurrentDateTime();
    cout << "Optional: Enter a message for your donation (press Enter for no message): ";
    getline(cin, newDonation.message);
    int new_num_donations = user.number_ofdonations + 1;
    donationn* temp_donations = new (std::nothrow) donationn[new_num_donations];
    if (temp_donations == nullptr) {
        cerr << "❌ Memory allocation failed for storing donation. Donation not processed." << endl;
        return;
    }
    for (int i = 0; i < user.number_ofdonations; ++i) {
        temp_donations[i] = user.donations[i];
    }
    temp_donations[user.number_ofdonations] = newDonation;
    if (user.donations != nullptr) {
        delete[] user.donations;
    }
    user.donations = temp_donations;
    charities[charityIndex].current_amount += donationAmount;
    charities[charityIndex].d = getCurrentDateTime();
    user.number_ofdonations++;
    cout << "✅ Thank you, " << user.first_name << "! Your donation of $" << fixed << setprecision(2) << donationAmount
         << " to '" << charities[charityIndex].name << "' has been processed." << endl;
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
        cout << "\nYou have not made any donations yet.\n";
        return;
    }
    cout << "\n--- Your Donation History ---" << endl;
    cout << left << setw(5) << "No."
         << setw(10) << "Don. ID"
         << setw(25) << "Charity Name"
         << right << setw(15) << "Amount ($)"
         << "  " << left << setw(12) << "Date"
         << setw(10) << "Time"
         << "  Message" << endl;
    cout << string(90, '-') << endl;
    for (int i = 0; i < user.number_ofdonations; ++i) {
        const donationn& d = user.donations[i];
        string charityName = "Unknown/Deleted";
        for(int j=0; j < totalCharityCount; ++j) {
            if (allCharities[j].charity_id == d.charity_id) {
                charityName = allCharities[j].name;
                break;
            }
        }
        if (charityName.length() >= 25) {
            charityName = charityName.substr(0, 22) + "...";
        }
        cout << left << setw(5) << (i + 1)
             << setw(10) << d.donation_id.substr(0,9)
             << setw(25) << charityName
             << right << setw(15) << fixed << setprecision(2) << d.amount
             << "  " << left << setw(12) << d.d.date
             << setw(10) << d.d.time;
        if (!d.message.empty()) {
            cout << "  Msg: " << (d.message.length() > 20 ? d.message.substr(0, 17) + "..." : d.message);
        }
        cout << endl;
    }
    cout << string(90, '-') << endl;
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
    donationn& donationToModify = user.donations[donationIndexToModify];
    float oldAmount = donationToModify.amount;
    int charityIdOfDonation = donationToModify.charity_id;
    string charityNameOfDonation = "Unknown";
     for(int j=0; j < charityCount; ++j) {
        if (charities[j].charity_id == charityIdOfDonation) {
            charityNameOfDonation = charities[j].name;
            break;
        }
    }
    cout << "Modifying your donation No. " << selection_num << " of $" << fixed << setprecision(2) << oldAmount
         << " to '" << charityNameOfDonation << "' (Donation ID: " << donationToModify.donation_id << ")." << endl;
    float newDonationAmount;
    float amount_left_to_target = -1.0f;
    int originalCharityIndex = -1;
    bool charity_has_target_limit = false;
    for (int i = 0; i < charityCount; ++i) {
        if (charities[i].charity_id == charityIdOfDonation) {
            originalCharityIndex = i;
            if (charities[i].target_amount > 0.0f) {
                charity_has_target_limit = true;
                if (charities[i].status) { // Active
                    amount_left_to_target = (charities[i].target_amount - charities[i].current_amount) + oldAmount;
                    cout << "Charity '" << charities[i].name << "' currently needs $" << fixed << setprecision(2)
                         << (charities[i].target_amount - charities[i].current_amount)
                         << " to reach its target. Max new amount for this donation: $" << fixed << setprecision(2) << amount_left_to_target << endl;
                } else { // Closed
                     cout << "Charity '" << charities[i].name << "' is CLOSED. You can only decrease this donation or keep it the same (max $" << fixed << setprecision(2) << oldAmount << ")." << endl;
                     amount_left_to_target = oldAmount;
                }
            }
            break;
        }
    }
    while (true) {
        cout << "Enter new donation amount: $"; string inputAmountStr; getline(cin, inputAmountStr);
        if (inputAmountStr.empty()){ cout << "No amount entered. Please try again." << endl; continue; }
        try { newDonationAmount = stof(inputAmountStr); }
        catch (const std::exception&) { cout << "Invalid number. Please try again." << endl; continue; }
        if (newDonationAmount <= 0) {
            cout << "New donation amount must be positive." << endl;
        } else if (charity_has_target_limit && newDonationAmount > amount_left_to_target) {
            cout << "New amount $" << fixed << setprecision(2) << newDonationAmount
                 << " is too high. Max allowed new amount for this donation is $"
                 << fixed << setprecision(2) << amount_left_to_target << "." << endl;
        } else {
            break;
        }
    }
    float amountDifference = newDonationAmount - oldAmount;
    donationToModify.amount = newDonationAmount;
    donationToModify.d = getCurrentDateTime();
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
    } else if (charityIdOfDonation != -1) {
        cout << "⚠️ Warning: Original charity (ID: " << charityIdOfDonation
             << ") for this donation was not found in the current charities list. Its total amount cannot be updated." << endl;
    }
    cout << "✅ Donation No. " << selection_num << " successfully modified to $"
         << fixed << setprecision(2) << newDonationAmount << ".\n";
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
    float cancelledAmount = user.donations[indexToCancel].amount;
    int charityIdOfCancelledDonation = user.donations[indexToCancel].charity_id;
    string donationIdCancelled = user.donations[indexToCancel].donation_id;
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
    cout << "✅ Donation successfully cancelled.\n";
}