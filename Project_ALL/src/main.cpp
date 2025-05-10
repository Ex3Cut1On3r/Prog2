// File: Project_ALL/src/main.cpp
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX

#include <iostream>
#include <string>
#include <limits>
#include <iomanip>
#include <windows.h>

#include "structsandfunctions/functions.h"
#include "structsandfunctions/structures.h"

using namespace std;

const WORD MAIN_DEFAULT_COLOR = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
const WORD MAIN_WELCOME_COLOR = FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY;
const WORD MAIN_MENU_BORDER_COLOR = FOREGROUND_BLUE | FOREGROUND_GREEN;
const WORD MAIN_MENU_TEXT_COLOR = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY;
const WORD MAIN_MENU_TITLE_COLOR = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY;
const WORD MAIN_PROMPT_COLOR = MAIN_DEFAULT_COLOR;
const WORD MAIN_ERROR_COLOR = FOREGROUND_RED | FOREGROUND_INTENSITY;
const WORD MAIN_SUCCESS_COLOR = FOREGROUND_GREEN | FOREGROUND_INTENSITY;
const WORD MAIN_INFO_COLOR = FOREGROUND_BLUE | FOREGROUND_INTENSITY;
const WORD MAIN_WARNING_COLOR = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY;

void setMainConsoleColor(WORD color) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, color);
}

int main() {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
#endif

    int choice;
    client loggedInClient;
    charity* charitiesArray = nullptr;
    int charityCount = 0;

    loadCharities(charitiesArray, charityCount);

    setMainConsoleColor(MAIN_WELCOME_COLOR);
    cout << "=============================================" << endl;
    cout << "   Welcome to the Donation Management System " << endl;
    cout << "=============================================" << endl;
    setMainConsoleColor(MAIN_DEFAULT_COLOR);

    while (true) {
        cout << "\n";
        setMainConsoleColor(MAIN_MENU_BORDER_COLOR);
        cout << "  o---------------------------------------o\n";
        cout << "  |                                       |\n";
        cout << "  |         ";
        setMainConsoleColor(MAIN_MENU_TITLE_COLOR);
        cout << "M A I N   M E N U";
        setMainConsoleColor(MAIN_MENU_BORDER_COLOR);
        cout << "             |\n";
        cout << "  |                                       |\n";
        cout << "  |=======================================|\n";
        cout << "  |                                       |\n";
        setMainConsoleColor(MAIN_MENU_TEXT_COLOR);
        cout << "  |      [1] Register New User            |\n";
        cout << "  |      [2] Login                        |\n";
        setMainConsoleColor(MAIN_MENU_BORDER_COLOR);
        cout << "  |                                       |\n";
        setMainConsoleColor(MAIN_MENU_TEXT_COLOR);
        cout << "  |      [0] Exit                         |\n";
        setMainConsoleColor(MAIN_MENU_BORDER_COLOR);
        cout << "  |                                       |\n";
        cout << "  o---------------------------------------o\n";
        setMainConsoleColor(MAIN_PROMPT_COLOR);
        cout << "  Enter your choice: ";
        setMainConsoleColor(MAIN_DEFAULT_COLOR);

        while (!(cin >> choice)) {
            setMainConsoleColor(MAIN_ERROR_COLOR);
            cout << "Invalid input. Please enter a number: ";
            setMainConsoleColor(MAIN_DEFAULT_COLOR);
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        if (choice == 1) {
            setMainConsoleColor(MAIN_INFO_COLOR);
            cout << "\n--- User Registration Initiated ---" << endl;
            setMainConsoleColor(MAIN_DEFAULT_COLOR);
            registerNewUser();
        } else if (choice == 2) {
            setMainConsoleColor(MAIN_INFO_COLOR);
            cout << "\n--- User Login ---" << endl;
            setMainConsoleColor(MAIN_DEFAULT_COLOR);
            string email, password;

            setMainConsoleColor(MAIN_PROMPT_COLOR);
            cout << "Enter your email: ";
            setMainConsoleColor(MAIN_DEFAULT_COLOR);
            getline(cin, email);

            setMainConsoleColor(MAIN_PROMPT_COLOR);
            cout << "Enter your password: ";
            setMainConsoleColor(MAIN_DEFAULT_COLOR);
            getline(cin, password);

            if (verifyUserLogin(email, password)) {
                setMainConsoleColor(MAIN_SUCCESS_COLOR);
                cout << "\nLogin Successful!" << endl;
                setMainConsoleColor(MAIN_DEFAULT_COLOR);
                loggedInClient = getClientDataByEmail(email);

                if (loggedInClient.user_id == -1) {
                     setMainConsoleColor(MAIN_ERROR_COLOR);
                     cout << "Error: Could not retrieve user data after login. Please contact support." << endl;
                     setMainConsoleColor(MAIN_DEFAULT_COLOR);
                     continue;
                }

                setMainConsoleColor(MAIN_INFO_COLOR);
                if (loggedInClient.role == "admin") {
                    cout << "Welcome Admin: " << loggedInClient.first_name << endl;
                    setMainConsoleColor(MAIN_DEFAULT_COLOR);
                    adminPanel(loggedInClient, charitiesArray, charityCount);
                } else if (loggedInClient.role == "user") {
                    cout << "Welcome User: " << loggedInClient.first_name << endl;
                    setMainConsoleColor(MAIN_DEFAULT_COLOR);
                    userPanel(loggedInClient, charitiesArray, charityCount);
                } else {
                    setMainConsoleColor(MAIN_WARNING_COLOR);
                    cout << "Warning: Unknown user role '" << loggedInClient.role << "'. Access denied." << endl;
                    setMainConsoleColor(MAIN_DEFAULT_COLOR);
                }
            } else {
                setMainConsoleColor(MAIN_ERROR_COLOR);
                cout << "Login Failed: Invalid email or password!" << endl;
                setMainConsoleColor(MAIN_DEFAULT_COLOR);
            }
        } else if (choice == 0) {
            setMainConsoleColor(MAIN_INFO_COLOR);
            cout << "Exiting program." << endl;
            setMainConsoleColor(MAIN_DEFAULT_COLOR);
            break;
        }
        else {
            setMainConsoleColor(MAIN_ERROR_COLOR);
            cout << "Invalid choice! Please try again." << endl;
            setMainConsoleColor(MAIN_DEFAULT_COLOR);
        }
    }

    setMainConsoleColor(MAIN_INFO_COLOR);
    cout << "Cleaning up resources..." << endl;
    setMainConsoleColor(MAIN_DEFAULT_COLOR);
    saveCharities(charitiesArray, charityCount);
    delete[] charitiesArray;
    charitiesArray = nullptr;
    setMainConsoleColor(MAIN_INFO_COLOR);
    cout << "Program finished." << endl;
    setMainConsoleColor(MAIN_DEFAULT_COLOR);
    return 0;
}