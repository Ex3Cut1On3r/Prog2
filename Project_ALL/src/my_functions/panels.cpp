#include "structsandfunctions/functions.h"
#include "structsandfunctions/exporttopdf.h"
#include <iostream>
#include <string>
#include <limits>
#include <fstream>
#include <filesystem>
#include <iomanip>

using std::cout;
using std::cin;
using std::endl;
using std::string;
using std::numeric_limits;
using std::streamsize;
using std::to_string;
using std::ifstream;
using std::setw;
using std::left;

const string T_BORDER_TL = "┌";
const string T_BORDER_TR = "┐";
const string T_BORDER_BL = "└";
const string T_BORDER_BR = "┘";
const string T_BORDER_H  = "─";
const string T_BORDER_V  = "│";
const string T_BORDER_LJ = "├";
const string T_BORDER_RJ = "┤";

void printMenuItem(int number, const string& text, int width) {
    cout << T_BORDER_V << " " << std::left << setw(3) << (to_string(number) + ".")
         << setw(width - 6) << text << " " << T_BORDER_V << endl;
}

void adminPanel(client& admin, charity*& charities, int& charityCount) {
    int choice;
    const int menu_width = 40;

    do {
        string header = " Admin Panel (" + admin.first_name + " " + admin.last_name + ") ";
        int header_padding_total = menu_width - header.length();
        int header_pad_left = (header_padding_total > 0) ? header_padding_total / 2 : 0;
        int header_pad_right = (header_padding_total > 0) ? header_padding_total - header_pad_left : 0;

        cout << endl;
        cout << T_BORDER_TL << string(menu_width, T_BORDER_H[0]) << T_BORDER_TR << endl;
        cout << T_BORDER_V << string(header_pad_left, ' ') << header << string(header_pad_right, ' ') << T_BORDER_V << endl;
        cout << T_BORDER_LJ << string(menu_width, T_BORDER_H[0]) << T_BORDER_RJ << endl;

        printMenuItem(1, "Add a charity", menu_width);
        printMenuItem(2, "Modify a charity", menu_width);
        printMenuItem(3, "Remove a charity", menu_width);
        printMenuItem(4, "View all charities", menu_width);
        cout << T_BORDER_LJ << string(menu_width, T_BORDER_H[0]) << T_BORDER_RJ << endl;
        printMenuItem(0, "Logout", menu_width);

        cout << T_BORDER_BL << string(menu_width, T_BORDER_H[0]) << T_BORDER_BR << endl;

        cout << "Enter choice: ";
        while (!(cin >> choice)) {
            cout << "Invalid input. Please enter a number: ";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Enter choice: ";
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        switch (choice) {
            case 1:
                addCharity(charities, charityCount);
                saveCharities(charities, charityCount);
                cout << "Charity added.\n";
                break;
            case 2:
                if (charityCount > 0) {
                    modifyCharity(charities, charityCount);
                    saveCharities(charities, charityCount);
                    cout << "Charity modified.\n";
                } else {
                    cout << "No charities to modify.\n";
                }
                break;
            case 3:
                if (charityCount > 0) {
                    removeCharity(charities, charityCount);
                    saveCharities(charities, charityCount);
                    cout << "Charity removed.\n";
                } else {
                    cout << "No charities to remove.\n";
                }
                break;
            case 4:
                displayCharities(charities, charityCount);
                break;
            case 0:
                cout << "Logging out from Admin Panel...\n";
                break;
            default:
                cout << "Invalid option. Try again.\n";
        }
        if (choice != 0) {
            cout << "\nPress Enter to continue...";
            cin.get();
        }
    } while (choice != 0);
}

void userPanel(client& user, charity*& charities, int& charityCount) {
    int choice;
    bool userDataHasChanged = false;
    const int menu_width = 45;

    do {
        string header = " User Panel (" + user.first_name + " " + user.last_name + ") ";
        string donations_info = "Donations made: " + to_string(user.number_ofdonations);
        int header_padding_total = menu_width - header.length();
        int header_pad_left = (header_padding_total > 0) ? header_padding_total / 2 : 0;
        int header_pad_right = (header_padding_total > 0) ? header_padding_total - header_pad_left : 0;
        int donations_info_padding_total = menu_width - donations_info.length();
        int donations_info_pad_left = (donations_info_padding_total > 0) ? donations_info_padding_total / 2 : 0;
        int donations_info_pad_right = (donations_info_padding_total > 0) ? donations_info_padding_total - donations_info_pad_left : 0;

        cout << endl;
        cout << T_BORDER_TL << string(menu_width, T_BORDER_H[0]) << T_BORDER_TR << endl;
        cout << T_BORDER_V << string(header_pad_left, ' ') << header << string(header_pad_right, ' ') << T_BORDER_V << endl;
        cout << T_BORDER_V << string(donations_info_pad_left, ' ') << donations_info << string(donations_info_pad_right, ' ') << T_BORDER_V << endl;
        cout << T_BORDER_LJ << string(menu_width, T_BORDER_H[0]) << T_BORDER_RJ << endl;

        printMenuItem(1, "Browse charities", menu_width);
        printMenuItem(2, "Make donation", menu_width);
        printMenuItem(3, "View my donations", menu_width);
        printMenuItem(4, "Cancel donation", menu_width);
        printMenuItem(5, "Modify donation", menu_width);
        printMenuItem(6, "Export My Donations to PDF", menu_width);
        cout << T_BORDER_LJ << string(menu_width, T_BORDER_H[0]) << T_BORDER_RJ << endl;
        printMenuItem(0, "Logout", menu_width);
        cout << T_BORDER_BL << string(menu_width, T_BORDER_H[0]) << T_BORDER_BR << endl;

        cout << "Enter choice: ";
        while (!(cin >> choice)) {
            cout << "Invalid input. Please enter a number: ";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Enter choice: ";
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        switch (choice) {
            case 1:
                displayCharities(charities, charityCount);
                break;
            case 2: {
                if (charityCount > 0) {
                    bool donationSuccessFlag = false;
                    makeDonation(user, charities, charityCount, donationSuccessFlag);
                    if (donationSuccessFlag) {
                        saveCharities(charities, charityCount);
                        userDataHasChanged = true;
                        cout << "Donation made successfully.\n";
                    }
                } else {
                    cout << "No charities to donate to.\n";
                }
                break;
            }
            case 3:
                cout << "Your donations:\n";
                displayUserDonations(user, charities, charityCount);
                break;
            case 4:
                if (user.number_ofdonations > 0) {
                    cancelDonation(user, charities, charityCount, charities, charityCount);
                    userDataHasChanged = true;
                    saveCharities(charities, charityCount);
                    cout << "Donation cancellation processed.\n";
                } else {
                    cout << "You have no donations to cancel.\n";
                }
                break;
            case 5: {
                if (user.number_ofdonations > 0) {
                    modifyDonation(user, charities, charityCount, charities, charityCount);
                    userDataHasChanged = true;
                    saveCharities(charities, charityCount);
                    cout << "Donation modification processed.\n";
                } else {
                    cout << "You have no donations to modify.\n";
                }
                break;
            }
            case 6: {
                string project_root_path = "C:/Users/Charlie/Documents/ProgrammingTwoProject/Programming2_Project-main/Project_ALL/";
                string my_pdfs_dir_path = project_root_path + "MY_Pdfs/";
                try {
                    if (!std::filesystem::exists(my_pdfs_dir_path)) {
                         if (!std::filesystem::create_directories(my_pdfs_dir_path)) {
                            cout << "Warning: Could not create PDF output directory: " << my_pdfs_dir_path << endl;
                         }
                    }
                } catch (const std::filesystem::filesystem_error& e) {
                    std::cerr << "Error with output directory " << my_pdfs_dir_path << ": " << e.what() << endl;
                    break;
                }
                string global_donations_json_path = project_root_path + "data/donations.json";
                string logo_file_path = project_root_path + "data/unilogo.png";
                string base_filename = "Donations_Report";
                if (!user.first_name.empty()) base_filename += "_" + user.first_name;
                if (!user.last_name.empty()) base_filename += "_" + user.last_name;
                for (char &c : base_filename) { if (c == ' ') c = '_'; }
                base_filename += "_" + to_string(user.user_id);

                string html_output_path = my_pdfs_dir_path + base_filename + ".html";
                string pdf_output_path = my_pdfs_dir_path + base_filename + ".pdf";
                cout << "Exporting donations to PDF for user: " << user.first_name << " " << user.last_name << " (ID: " << user.user_id << ")...\n";

                export_to_html(
                    global_donations_json_path,
                    html_output_path,
                    logo_file_path,
                    user.user_id,
                    user.first_name + " " + user.last_name
                );

                ifstream test_html_file(html_output_path);
                if (test_html_file.is_open()) {
                    test_html_file.close();
                    if (convert_html_to_pdf(html_output_path, pdf_output_path)) {
                        cout << "PDF report exported to: " << pdf_output_path << endl;
                    } else {
                        std::cerr << "Error: PDF conversion failed." << endl;
                    }
                } else {
                    std::cerr << "Error: Could not create intermediate HTML file: " << html_output_path << endl;
                }
                break;
            }
            case 0:
                cout << "Logging out from User Panel...\n";
                break;
            default:
                cout << "Invalid option. Try again.\n";
        }
        if (choice != 0) {
            cout << "\nPress Enter to continue...";
            cin.get();
        }
    } while (choice != 0);

    if (userDataHasChanged) {
        saveUserData(user);
        cout << "User data saved.\n";
    }
}