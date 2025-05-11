#ifndef FUNCTIONS_H
#define FUNCTIONS_H
#include <string>
#include "structures.h"
datee getCurrentDateTime();
void loadCharities(charity*& charities, int& charityCount);
void saveCharities(const charity* charities, int charityCount);
void addCharity(charity*& charities, int& charityCount);
void modifyCharity(charity*& charities, int charityCount);
void removeCharity(charity*& charities, int& charityCount);
void displayCharities(const charity* charities, int charityCount);
void makeDonation(client& user, charity*& charities, int charityCount, bool& successFlag);
void displayUserDonations(const client& user, const charity* allCharities, int totalCharityCount);
void cancelDonation(client& user, charity*& charities, int charityCount, const charity* allCharities, int totalCharityCount);
void modifyDonation(client& user, charity*& charities, int charityCount, const charity* allCharities, int totalCharityCount);
client registerNewUser();
bool verifyUserLogin(const std::string& email, const std::string& password);
client getClientDataByEmail(const std::string& email);
void saveUserData(const client& userToSave);
bool checkIfAdmin(const std::string& email);
void adminPanel(client  & admin, charity*& charities, int& charityCount);
void userPanel(client& user, charity*& charities, int& charityCount);
#endif