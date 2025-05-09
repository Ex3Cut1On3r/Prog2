// File: Project_ALL/src/my_functions/panels.cpp

// Include headers for declarations of functions and types used in this file
// Adjust these paths based on your actual project structure and how
// you've set up `include_directories` in CMakeLists.txt.
// Assuming `include_directories(Project_ALL)` in CMakeLists.txt:
#include "structsandfunctions/functions.h"   // Should declare client, charity, and functions like displayCharities, makeDonation etc.
#include "structsandfunctions/exporttopdf.h" // Declares export_to_html and convert_html_to_pdf
#include <iostream> // For std::cout, std::cin
#include <string>   // For std::string
#include <limits>   // For std::numeric_limits
#include <fstream>  // For std::ifstream to check HTML file existence (optional check)
#include <filesystem> // For std::filesystem::create_directories, std::filesystem::exists, std::filesystem::current_path (C++17)


// If client, charity, etc., are not fully declared in functions.h,
// you might need to include their specific header (e.g., "src/structsandfunctions/structures.h")
// For now, assuming functions.h covers all necessary type declarations from structsandfunctions.

// Function prototypes for functions called within panels but potentially defined elsewhere
// These would typically be in functions.h or another shared header.
// If they are already in "src/structsandfunctions/functions.h", you don't need to redeclare them here.
/*
void addCharity(charity*& charities, int& charityCount);
void modifyCharity(charity*& charities, int& charityCount);
void removeCharity(charity*& charities, int& charityCount);
void displayCharities(const charity* charities, int charityCount); // const if not modified
void saveCharities(const charity* charities, int charityCount);   // const if not modified
void makeDonation(client& user, charity* charities, int charityCount, bool& successFlag);
void displayUserDonations(const client& user); // const if not modified
void cancelDonation(client& user, charity* charities, int charityCount);
void modifyDonation(client& user, charity* charities, int charityCount);
void saveUserData(const client& user); // const if not modified
*/


void adminPanel(client& admin, charity*& charities, int& charityCount) {
    int choice;
    do {
        std::cout << "\n--- Admin Panel (" << admin.first_name << " " << admin.last_name << ") ---\n";
        std::cout << "1. Add a charity\n2. Modify a charity\n3. Remove a charity\n4. View all charities\n0. Logout\n";
        std::cout << "Enter choice: ";
        while (!(std::cin >> choice)) {
            std::cout << "Invalid input. Please enter a number: ";
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Consume trailing newline

        switch (choice) {
            case 1:
                // addCharity(charities, charityCount);
                // saveCharities(charities, charityCount);
                std::cout << "Admin: Add Charity (Placeholder)\n";
                break;
            case 2:
                // if (charityCount > 0) { modifyCharity(charities, charityCount); saveCharities(charities, charityCount); }
                // else { std::cout << "No charities to modify.\n"; }
                std::cout << "Admin: Modify Charity (Placeholder)\n";
                break;
            case 3:
                // if (charityCount > 0) { removeCharity(charities, charityCount); saveCharities(charities, charityCount); }
                // else { std::cout << "No charities to remove.\n"; }
                std::cout << "Admin: Remove Charity (Placeholder)\n";
                break;
            case 4:
                // displayCharities(charities, charityCount);
                std::cout << "Admin: View All Charities (Placeholder)\n";
                break;
            case 0:
                std::cout << "Logging out from Admin Panel...\n";
                break;
            default:
                std::cout << "Invalid option. Try again.\n";
        }
    } while (choice != 0);
}

void userPanel(client& user, charity*& charities, int& charityCount) {
    int choice;
    bool userDataHasChanged = false; // To track if user data needs saving on logout
    do {
        std::cout << "\n--- User Panel (" << user.first_name << " " << user.last_name << ") ---\n";
        // Example: if client struct has a number_ofdonations member
        // if (user.donations_list != nullptr) { // Assuming donations_list is a pointer or similar
        //    std::cout << "Donations made: " << user.number_ofdonations << "\n";
        // } else {
        //    std::cout << "Donations made: 0\n";
        // }
        std::cout << "1. Browse charities\n2. Make donation\n3. View my donations\n";
        std::cout << "4. Cancel donation\n5. Modify donation\n";
        std::cout << "6. Export All Donations to PDF\n";
        std::cout << "0. Logout\n";
        std::cout << "Enter choice: ";

        while (!(std::cin >> choice)) {
            std::cout << "Invalid input. Please enter a number: ";
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Consume trailing newline

        switch (choice) {
            case 1:
                // displayCharities(charities, charityCount);
                std::cout << "User: Browse Charities (Placeholder)\n";
                break;
            case 2: {
                // if (charityCount > 0) {
                //     bool donationSuccessFlag = false;
                //     makeDonation(user, charities, charityCount, donationSuccessFlag);
                //     if (donationSuccessFlag) {
                //         saveCharities(charities, charityCount); // Or save all data
                //         userDataHasChanged = true;
                //     }
                // } else { std::cout << "No charities to donate to.\n"; }
                std::cout << "User: Make Donation (Placeholder)\n";
                break;
            }
            case 3:
                // displayUserDonations(user);
                std::cout << "User: View My Donations (Placeholder)\n";
                break;
            case 4:
                // cancelDonation(user, charities, charityCount); // Implement this
                // userDataHasChanged = true; // If a donation is successfully cancelled
                // saveCharities(charities, charityCount); // Or save all data
                std::cout << "User: Cancel Donation (Placeholder)\n";
                break;
            case 5: {
                // modifyDonation(user, charities, charityCount); // Implement this
                // userDataHasChanged = true; // If a donation is successfully modified
                // saveCharities(charities, charityCount); // Or save all data
                std::cout << "User: Modify Donation (Placeholder)\n";
                break;
            }
            case 6: { // Export to PDF
                // **1. Define the base path to your Project_ALL directory**
                //    Update this path if your project is located elsewhere.
                std::string project_root_path = "C:/Users/Charlie/Documents/nigga/Programming2_Project-main/Project_ALL/"; // Ensure trailing slash

                // **2. Construct the path to your MY_Pdfs folder**
                std::string my_pdfs_dir_path = project_root_path + "MY_Pdfs/";

                // **3. (Recommended) Create the MY_Pdfs directory if it doesn't exist**
                try {
                    if (!std::filesystem::exists(my_pdfs_dir_path)) {
                        if (std::filesystem::create_directories(my_pdfs_dir_path)) {
                            std::cout << "INFO: Created directory: " << my_pdfs_dir_path << std::endl;
                        } else {
                            // This case might happen if create_directories fails for reasons other than it already existing,
                            // or if another process created it between exists() and create_directories().
                            std::cout << "WARNING: Could not create directory (it might exist or other issue): " << my_pdfs_dir_path << std::endl;
                        }
                    }
                } catch (const std::filesystem::filesystem_error& e) {
                    std::cerr << "ERROR: Filesystem error while checking/creating directory " << my_pdfs_dir_path << ": " << e.what() << std::endl;
                    std::cerr << "Please ensure the path is valid and you have write permissions." << std::endl;
                    break; // Exit this case if directory handling fails critically
                }

                std::string json_input_path = project_root_path + "data/donations.json";

                // *** Define the path to your logo ***
                // *** IMPORTANT: Adjust this path to where your unilogo.png is actually located. ***
                // Examples:
                // std::string logo_file_path = project_root_path + "resources/unilogo.png"; // If in Project_ALL/resources/
                // std::string logo_file_path = project_root_path + "unilogo.png";      // If directly in Project_ALL/
                // For demonstration, assuming it's in a 'data' subfolder alongside donations.json
                std::string logo_file_path = project_root_path + "data/unilogo.png";   // <<<< ADJUST THIS PATH!!!

                // Customize output filenames using user's name
                std::string base_filename = "Donations_Report";
                if (!user.first_name.empty()) base_filename += "_" + user.first_name;
                if (!user.last_name.empty()) base_filename += "_" + user.last_name;
                // Sanitize filename (simple example, replace spaces; more robust would remove other invalid chars)
                for (char &c : base_filename) { if (c == ' ') c = '_'; }


                std::string html_output_path = my_pdfs_dir_path + base_filename + ".html";
                std::string pdf_output_path = my_pdfs_dir_path + base_filename + ".pdf";


                std::cout << "INFO: User selected option 6 (Export All Donations to PDF)." << std::endl;
                std::cout << "INFO: Using JSON source file path: [" << json_input_path << "]" << std::endl;
                std::cout << "INFO: Using Logo file path: [" << logo_file_path << "]" << std::endl;
                std::cout << "INFO: HTML output will be: [" << html_output_path << "]" << std::endl;
                std::cout << "INFO: PDF output will be: [" << pdf_output_path << "]" << std::endl;

                // Call export_to_html with the logo path
                export_to_html(json_input_path, html_output_path, logo_file_path);

                // Optional: Check if HTML file was actually created
                std::ifstream test_html_file(html_output_path);
                if (test_html_file.is_open()) {
                    test_html_file.close();
                    std::cout << "INFO: Intermediate HTML file [" << html_output_path << "] created successfully. Attempting PDF conversion." << std::endl;

                    if (convert_html_to_pdf(html_output_path, pdf_output_path)) {
                        std::cout << "SUCCESS: PDF report exported successfully to [" << pdf_output_path << "]." << std::endl;
                    } else {
                        std::cout << "ERROR: PDF conversion function reported failure." << std::endl;
                    }
                } else {
                    std::cout << "ERROR: Intermediate HTML file [" << html_output_path << "] was NOT created by export_to_html or is not accessible." << std::endl;
                    // Debugging current working directory can be helpful
                    try {
                       std::cout << "DEBUG: Current working directory: " << std::filesystem::current_path() << std::endl;
                    } catch (const std::filesystem::filesystem_error& e) {
                       std::cerr << "DEBUG: Error getting current path: " << e.what() << std::endl;
                    }
                }
                break;
            }
            case 0:
                std::cout << "Logging out from User Panel...\n";
                break;
            default:
                std::cout << "Invalid option. Try again.\n";
        }
    } while (choice != 0);

    if (userDataHasChanged) {
        // saveUserData(user); // Call your function to save any changes to the user's data
        std::cout << "INFO: User data changes saved (Placeholder for actual saveUserData call).\n";
    }
}