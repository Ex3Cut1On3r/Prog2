// File: Project_ALL/src/main.cpp
#include <iostream>
#include <string>
#include <limits>
#include <iomanip> // If main does any direct formatting

#include "structsandfunctions/functions.h" // Includes all necessary declarations from your central header
#include "structsandfunctions/structures.h"
using namespace std;

int main() {
    int choice;
    client loggedInClient;
    charity* charitiesArray = nullptr;
    int charityCount = 0;

    loadCharities(charitiesArray, charityCount);

    cout << "=============================================" << endl;
    cout << "   Welcome to the Donation Management System " << endl;
    cout << "=============================================" << endl;

    while (true) {
        cout << "\n--- Main Menu ---" << endl;
        cout << "1. Register New User" << endl;
        cout << "2. Login" << endl;
        cout << "0. Exit" << endl;
        cout << "Enter your choice: ";

        while (!(cin >> choice)) {
            cout << "Invalid input. Please enter a number: ";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        if (choice == 1) {
            cout << "\n--- User Registration ---" << endl;
            registerNewUser();
            cout << "Registration Successful! Please login." << endl;
        } else if (choice == 2) {
            cout << "\n--- User Login ---" << endl;
            string email, password;
            cout << "Enter your email: ";
            getline(cin, email);
            cout << "Enter your password: ";
            getline(cin, password);

            if (verifyUserLogin(email, password)) {
                cout << "\nLogin Successful!" << endl;
                loggedInClient = getClientDataByEmail(email);

                if (loggedInClient.user_id == -1) {
                     cout << "Error: Could not retrieve user data after login. Please contact support." << endl;
                     continue;
                }
                if (loggedInClient.role == "admin") {
                    cout << "Welcome Admin: " << loggedInClient.first_name << endl;
                    adminPanel(loggedInClient, charitiesArray, charityCount);
                } else if (loggedInClient.role == "user") {
                    cout << "Welcome User: " << loggedInClient.first_name << endl;
                    userPanel(loggedInClient, charitiesArray, charityCount);
                } else {
                    cout << "Warning: Unknown user role '" << loggedInClient.role << "'. Access denied." << endl;
                }
            } else {
                cout << "Login Failed: Invalid email or password!" << endl;
            }
        } else if (choice == 0) {
            cout << "Exiting program." << endl;
            break;
        }
        else {
            cout << "Invalid choice! Please try again." << endl;
        }
    }

    cout << "Cleaning up resources..." << endl;
    saveCharities(charitiesArray, charityCount);
    delete[] charitiesArray;
    charitiesArray = nullptr;
    cout << "Program finished." << endl;
    return 0;
}