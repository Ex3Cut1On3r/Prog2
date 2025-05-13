#include "structsandfunctions/functions.h"
#include "structsandfunctions/exporttopdf.h"
#include <iostream>
#include <string>
#include <limits>
#include <fstream>
#include <filesystem>
#include <vector> // For std::to_string with some compilers, or if filesystem needs it indirectly

using std::cout;
using std::cin;
using std::endl;
using std::string;
using std::numeric_limits;
using std::streamsize;
using std::to_string; // Important for converting user.user_id to string for filename
using std::ifstream;
// Removed using namespace std; to be more explicit

void adminPanel(client& admin, charity*& charities, int& charityCount) {
    int choice;
    do {
        cout << "\n--- Admin Panel (" << admin.first_name << " " << admin.last_name << ") ---\n";
        cout << "1. Add a charity\n2. Modify a charity\n3. Remove a charity\n4. View all charities\n0. Logout\n";
        cout << "Enter choice: ";
        while (!(cin >> choice)) {
            cout << "Invalid input. Please enter a number: ";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
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
    } while (choice != 0);
}

void userPanel(client& user, charity*& charities, int& charityCount) {
    int choice;
    bool userDataHasChanged = false;
    do {
        cout << "\n--- User Panel (" << user.first_name << " " << user.last_name << ") ---\n";
        cout << "Donations made: " << user.number_ofdonations << "\n";
        cout << "1. Browse charities\n2. Make donation\n3. View my donations\n";
        cout << "4. Cancel donation\n5. Modify donation\n";
        cout << "6. Export My Donations to PDF\n"; // Changed wording slightly
        cout << "0. Logout\n";
        cout << "Enter choice: ";

        while (!(cin >> choice)) {
            cout << "Invalid input. Please enter a number: ";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
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
                    cerr << "Error with output directory " << my_pdfs_dir_path << ": " << e.what() << endl;
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
                        cerr << "Error: PDF conversion failed." << endl;
                    }
                } else {
                    cerr << "Error: Could not create intermediate HTML file: " << html_output_path << endl;
                }
                break;
            }
            case 0:
                cout << "Logging out from User Panel...\n";
                break;
            default:
                cout << "Invalid option. Try again.\n";
        }
    } while (choice != 0);

    if (userDataHasChanged) {
        saveUserData(user);
        cout << "User data saved.\n";
    }
}