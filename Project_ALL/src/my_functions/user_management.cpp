// File: Project_ALL/src/my_functions/user_management.cpp

// Assuming nlohmann/json.hpp is at Project_ALL/src/nlohman/json.hpp
#include "nlohman/json.hpp" // Corrected include path

// Assuming bcrypt.h is at Project_ALL/src/MY_hashingthingys/bcrypt.h
#include "MY_hashingthingys/bcrypt.h" // Corrected include path

// Assuming structures.h (which should define 'client' struct) is at Project_ALL/src/structsandfunctions/structures.h
#include "structsandfunctions/structures.h" // Corrected include path (removed trailing slash)

#include <iostream>
#include <fstream>
#include <iomanip>  // For std::setw
#include <limits>   // For std::numeric_limits
#include <string>
#include <vector>   // Often useful, include if needed elsewhere
#include <regex>    // For std::regex and std::regex_match


// It's generally better to qualify std:: names (e.g., std::cout) or use specific using declarations.
// using namespace std; // Avoid if possible, especially in headers. Okay in .cpp if you're careful.
using json = nlohmann::json; // This is fine.

// --- Path to users.json ---
// Define this ONCE, perhaps as a global constant within this file, or pass it around.
// For now, let's make it a constant here.
// THIS IS THE ABSOLUTE PATH BASED ON YOUR PREVIOUS DEBUGGING.
const std::string USERS_JSON_FILE_PATH = "C:/Users/Charlie/Documents/nigga/Programming2_Project-main/Project_ALL/data/users.json";


bool isValidEmailFormat(const std::string& email) { // Changed to std::string
    // A more common and slightly more robust regex for basic email validation
    const std::regex pattern(R"(^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,}$)");
    return std::regex_match(email, pattern);
}

client getClientDataByEmail(const std::string& email) { // Changed to std::string
    client foundClient;
    foundClient.user_id = -1; // Initialize to indicate not found

    std::ifstream inputFile(USERS_JSON_FILE_PATH); // Use the constant path
    if (!inputFile.is_open()) {
        // std::cerr << "DEBUG: getClientDataByEmail - Could not open users.json at: " << USERS_JSON_FILE_PATH << std::endl;
        return foundClient;
    }

    json users_json_data;
    try {
        inputFile >> users_json_data;
        if (!users_json_data.is_array()) {
            // std::cerr << "DEBUG: getClientDataByEmail - users.json is not an array." << std::endl;
            inputFile.close();
            return foundClient;
        }
    } catch (const json::parse_error& e) {
        std::cerr << "ERROR: getClientDataByEmail - JSON parse error in users.json: " << e.what() << std::endl;
        inputFile.close();
        return foundClient;
    } catch (const std::exception& e) { // Catch other potential exceptions during read
        std::cerr << "ERROR: getClientDataByEmail - Exception reading users.json: " << e.what() << std::endl;
        inputFile.close();
        return foundClient;
    }
    inputFile.close();

    for (const auto& user_json_obj : users_json_data) {
        if (!user_json_obj.is_object() || !user_json_obj.contains("email")) {
            continue;
        }
        if (user_json_obj.value("email", "") == email) {
            try {
                foundClient.user_id = user_json_obj.value("user_id", -1);
                foundClient.first_name = user_json_obj.value("first_name", "");
                foundClient.last_name = user_json_obj.value("last_name", "");
                foundClient.email = user_json_obj.value("email", ""); // Already have this, but good to populate from source
                foundClient.phone = user_json_obj.value("phone", "");
                foundClient.password = user_json_obj.value("password", ""); // This is the hashed password
                foundClient.role = user_json_obj.value("role", "user");
                if (user_json_obj.contains("number_ofdonations") && user_json_obj["number_ofdonations"].is_number_integer()) {
                    foundClient.number_ofdonations = user_json_obj["number_ofdonations"].get<int>();
                } else {
                    foundClient.number_ofdonations = 0;
                }
            } catch (const json::type_error& e) {
                std::cerr << "⚠️  Type error retrieving user data for email " << email << ": " << e.what() << std::endl;
                foundClient.user_id = -1; // Reset if parsing fields fails
            }
            break; // Found the user
        }
    }
    return foundClient;
}

client registerNewUser() {
    client newUser;
    std::string plain_password_input; // Use a different variable for plain text input
    // string filePath = "../Project_ALL/data/users.json"; // Replaced by USERS_JSON_FILE_PATH
    json users_json_array = json::array(); // Start with an empty array

    std::cout << "\n--- User Registration ---" << std::endl;

    std::cout << "Enter First Name: ";
    std::getline(std::cin, newUser.first_name);

    std::cout << "Enter Last Name: ";
    std::getline(std::cin, newUser.last_name);

    while (true) {
        std::cout << "Enter Email: ";
        std::getline(std::cin, newUser.email);
        if (isValidEmailFormat(newUser.email)) {
            client existingUser = getClientDataByEmail(newUser.email); // Checks against current file content
            if (existingUser.user_id == -1) { // Email not found
                break;
            } else {
                std::cout << "❌ This email address is already registered. Please use a different email or login.\n";
            }
        } else {
            std::cout << "❌ Invalid email format. Please try again (e.g., user@example.com).\n";
        }
    }

    std::cout << "Enter Phone: ";
    std::getline(std::cin, newUser.phone);

    while(true){
        std::cout << "Enter password (min 8 characters): ";
        std::getline(std::cin, plain_password_input);
        if(plain_password_input.length() >= 8) {
            break;
        }
        std::cout << "Password too short. Please try again.\n";
    }
    // Hash the plain password
    newUser.password = bcrypt::generateHash(plain_password_input);

    std::cout << "Enter Role (user/admin - default is 'user'): ";
    std::getline(std::cin, newUser.role);
    if (newUser.role != "admin" && newUser.role != "user") {
        std::cout << "Invalid role. Defaulting to 'user'.\n";
        newUser.role = "user";
    }

    // Read existing users to append
    std::ifstream inputFile(USERS_JSON_FILE_PATH);
    if (inputFile.is_open()) {
        try {
            inputFile >> users_json_array;
            // Ensure it's an array; if not, or if parse fails, users_json_array will be reset
            if (!users_json_array.is_array()) {
                 std::cerr << "⚠️  Existing users.json is not a JSON array. Starting fresh for append." << std::endl;
                 users_json_array = json::array(); // Reset to empty array
            }
        } catch (const json::parse_error&) {
            std::cerr << "⚠️  Error parsing existing users.json. Will create a new file or overwrite if it was malformed." << std::endl;
            users_json_array = json::array(); // Reset to empty array on parse error
        }
        inputFile.close();
    } else {
        std::cout << "INFO: users.json not found. A new file will be created." << std::endl;
        // users_json_array is already json::array()
    }


    // Determine new user ID
    int max_user_id = 0;
    for (const auto& user_json_obj : users_json_array) {
        if (user_json_obj.is_object() && user_json_obj.contains("user_id") && user_json_obj["user_id"].is_number_integer()) {
            if (user_json_obj["user_id"].get<int>() > max_user_id) {
                max_user_id = user_json_obj["user_id"].get<int>();
            }
        }
    }
    newUser.user_id = max_user_id + 1;
    newUser.number_ofdonations = 0; // New users start with 0 donations

    // Create JSON object for the new user
    json user_json_to_add = {
        {"user_id", newUser.user_id},
        {"first_name", newUser.first_name},
        {"last_name", newUser.last_name},
        {"email", newUser.email},
        {"phone", newUser.phone},
        {"password", newUser.password}, // Store the hashed password
        {"role", newUser.role},
        {"number_ofdonations", newUser.number_ofdonations}
    };
    users_json_array.push_back(user_json_to_add); // Add new user to the array

    // Write the updated array back to the file
    std::ofstream outputFile(USERS_JSON_FILE_PATH);
    if (!outputFile.is_open()) {
        std::cerr << "❌ CRITICAL: Could not open '" << USERS_JSON_FILE_PATH << "' for writing! New user not saved.\n";
        newUser.user_id = -1; // Indicate failure
    } else {
        outputFile << std::setw(4) << users_json_array << std::endl; // Pretty print
        if (outputFile.fail()) { // Check for write errors
             std::cerr << "❌ CRITICAL: Error writing new user to '" << USERS_JSON_FILE_PATH << "'. User not saved.\n";
             newUser.user_id = -1; // Indicate failure
        } else {
            std::cout << "✅ User '" << newUser.email << "' registered successfully! User ID: " << newUser.user_id << "\n";
        }
        outputFile.close();
    }
    return newUser;
}

bool verifyUserLogin(const std::string& email, const std::string& plain_password_input) { // Changed to std::string
    // string filePath = "../Project_ALL/data/users.json"; // Replaced
    std::ifstream inputFile(USERS_JSON_FILE_PATH);
    if (!inputFile.is_open()) {
        // std::cerr << "DEBUG: verifyUserLogin - Could not open users.json at: " << USERS_JSON_FILE_PATH << std::endl;
        return false;
    }

    json users_json_data;
    try {
        inputFile >> users_json_data;
        if (!users_json_data.is_array()) {
            inputFile.close();
            return false;
        }
    } catch (const std::exception&) { // Catch parse errors or other issues
        inputFile.close();
        return false;
    }
    inputFile.close();

    for (const auto& user_obj : users_json_data) {
        if (user_obj.is_object() && user_obj.contains("email") && user_obj["email"].is_string() &&
            user_obj.contains("password") && user_obj["password"].is_string()) {
             if (user_obj.value("email","") == email) {
                 std::string stored_hashed_password = user_obj.value("password","");
                 // Use bcrypt to validate the provided plain_password_input against the stored hash
                 if (bcrypt::validatePassword(plain_password_input, stored_hashed_password)) {
                     return true; // Password matches
                 }
                 return false; // Password does not match for this email
             }
        }
    }
    return false; // Email not found
}

void saveUserData(const client& userToSave) {
    // string filePath = "../Project_ALL/data/users.json"; // Replaced
    json users_json_array;
    std::ifstream inputFile(USERS_JSON_FILE_PATH);

    if (inputFile.is_open()) {
        try {
            inputFile >> users_json_array;
            if (!users_json_array.is_array()) {
                std::cerr << "⚠️  users.json is not a valid JSON array. Aborting save for user: " << userToSave.email << "." << std::endl;
                inputFile.close();
                return;
            }
        } catch (const json::parse_error& e) {
            std::cerr << "⚠️  Error parsing users.json: " << e.what() << ". Save for user: " << userToSave.email << " aborted." << std::endl;
            inputFile.close();
            return;
        }
        inputFile.close();
    } else {
        std::cout << "[Info] users.json not found while trying to save data for user: " << userToSave.email << ". This should not happen if user exists." << std::endl;
        // If the file doesn't exist, we can't update a specific user.
        // This function assumes the user being saved *should* exist in the file.
        // If you want to create the file if it doesn't exist and add this user, the logic needs to change.
        // For an update, the file must exist.
        return;
    }

    bool userFoundAndUpdated = false;
    for (auto& user_json_obj : users_json_array) { // Note: use reference '&' to modify in place
        if (user_json_obj.is_object() && user_json_obj.contains("user_id") &&
            user_json_obj.value("user_id", -999) == userToSave.user_id) { // Compare with a sentinel not likely to be a real ID

            // Update all fields from userToSave object
            user_json_obj["first_name"] = userToSave.first_name;
            user_json_obj["last_name"] = userToSave.last_name;
            user_json_obj["email"] = userToSave.email; // Email might change, ensure it's still unique if required elsewhere
            user_json_obj["phone"] = userToSave.phone;
            // Password is NOT updated here. Password changes should be a separate, secure process.
            // user_json_obj["password"] = userToSave.password; // DO NOT DO THIS unless userToSave.password is already re-hashed
            user_json_obj["role"] = userToSave.role;
            user_json_obj["number_ofdonations"] = userToSave.number_ofdonations;

            userFoundAndUpdated = true;
            break;
        }
    }

    if (!userFoundAndUpdated) {
        std::cerr << "CRITICAL ⚠️ : User ID " << userToSave.user_id << " (Email: " << userToSave.email
                  << ") NOT FOUND in users.json during save. Data for this user was NOT saved. Check logic." << std::endl;
        return; // Don't rewrite the file if the user to update wasn't found
    }

    std::ofstream outputFile(USERS_JSON_FILE_PATH);
    if (!outputFile.is_open()) {
        std::cerr << "❌ CRITICAL: Could not open '" << USERS_JSON_FILE_PATH << "' for writing! User data for " << userToSave.email << " not saved.\n";
        return;
    }

    outputFile << std::setw(4) << users_json_array << std::endl;
    if (outputFile.fail()) {
        std::cerr << "❌ CRITICAL: Error writing updated user data to '" << USERS_JSON_FILE_PATH << "' for " << userToSave.email << ".\n";
    } else {
        std::cout << "[Info] User data for " << userToSave.email << " (ID: " << userToSave.user_id << ") successfully saved to users.json." << std::endl;
    }
    outputFile.close();
}

bool checkIfAdmin(const std::string& email) { // Changed to std::string
    client user = getClientDataByEmail(email);
    return user.user_id != -1 && user.role == "admin";
}