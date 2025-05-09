#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <string>
#include "structures.h" // Make sure this defines client, charity, datee

// Using namespace std; // It's generally better to avoid "using namespace std;" in header files.
// Instead, qualify with std::, e.g., std::string.
// However, if your .cpp files rely on it, you can keep it for now, but be aware of potential naming conflicts.

// --- Date/Time ---
datee getCurrentDateTime();

// --- Charity Management ---
void loadCharities(charity*& charities, int& charityCount);
void saveCharities(const charity* charities, int charityCount); // Added const
void addCharity(charity*& charities, int& charityCount);
void modifyCharity(charity* charities, int charityCount);
void removeCharity(charity*& charities, int& charityCount);
void displayCharities(const charity* charities, int charityCount); // Added const

// --- Donation Management ---
// Corrected to match the 4-parameter call that includes donationSuccessFlag
void makeDonation(client& user, charity*& charities, int& charityCount, bool& donationMadeSuccessfully);
void displayUserDonations(const client& user); // Added const
// Corrected to match 3-parameter call
void cancelDonation(client& user, charity*& charities, int& charityCount);
void modifyDonation(client& user, charity*& charities, int& charityCount);
// --- User Management ---
client registerNewUser();
bool verifyUserLogin(const std::string& email, const std::string& password);
client getClientDataByEmail(const std::string& email);
void saveUserData(const client& userToSave); // ADDED DECLARATION
bool checkIfAdmin(const std::string& email); // UNCOMMENTED - Assuming it's defined and used

// --- Panels ---
void adminPanel(client  & admin, charity*& charities, int& charityCount);
void userPanel(client& user, charity*& charities, int& charityCount);
#endif // FUNCTIONS_H