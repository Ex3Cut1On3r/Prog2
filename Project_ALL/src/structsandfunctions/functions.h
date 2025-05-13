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
bool verifyUserLogin(const std::string& email, const std::string& plain_password_input);
client getClientDataByEmail(const std::string& email);
void saveUserData(const client& userToSave);
bool checkIfAdmin(const std::string& email);

void adminPanel(client& admin, charity*& charities, int& charityCount);
void userPanel(client& user, charity*& charities, int& charityCount);
// In functions.h or exporttopdf.h
void export_to_html(
    const std::string& json_file_path,
    const std::string& html_file_path,
    const std::string& logo_path_param,
    int target_user_id,
    const std::string& target_user_name
);
// Add declarations for exporttopdf.h functions if needed globally
// bool export_to_html(const std::string& json_file_path, const std::string& html_file_path, const std::string& logo_path);
// bool convert_html_to_pdf(const std::string& html_file_path, const std::string& pdf_file_path);

#endif // FUNCTIONS_H