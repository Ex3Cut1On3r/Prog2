#ifndef STRUCTURES_H
#define STRUCTURES_H
#include <string>

using namespace std;
struct datee {
    string date;
    string time;
};
struct donationn {
    string donation_id;
    string charity_id;
    float amount;
    datee d;
    string message;
};
struct charity {
    int charity_id;
    string name;
    string description;
    float target_amount;
    float current_amount;
    datee d;
    string status; // --> betkun ya this or that
};
struct client {
    int user_id;
    string first_name;
    string last_name;
    string email;
    string password;
    string phone;
    int number_ofdonations;
    donationn* donations; // --since i cant use verctors i used a dynamic arayy
    string role; //i8sa admin or donor
};
client registerNewUser();

#endif