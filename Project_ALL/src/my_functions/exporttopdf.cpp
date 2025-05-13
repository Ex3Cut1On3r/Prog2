#include "structsandfunctions/exporttopdf.h"
#include "nlohman/json.hpp"
#include <fstream>
#include <iostream>
#include <string>
#include <cstdlib>
#include <iomanip>
#include <sstream>
#include <chrono>
#include <ctime>
#include <new> // Required for std::nothrow

using std::string;
using std::cout;
using std::cerr;
using std::endl;
using std::ifstream;
using std::ofstream;
using std::ostringstream;
using std::setprecision;
using std::fixed;
using std::to_string;
using json = nlohmann::json;

string format_timestamp(long long unix_timestamp) {
    if (unix_timestamp <= 0) {
        return "N/A";
    }
    auto time_point = std::chrono::system_clock::from_time_t(static_cast<std::time_t>(unix_timestamp));
    std::time_t time_tt = std::chrono::system_clock::to_time_t(time_point);

    std::tm gmt_tm_struct = {};
#ifdef _MSC_VER
    if (gmtime_s(&gmt_tm_struct, &time_tt) != 0) {
        return "Timestamp Conversion Error (gmtime_s)";
    }
#else
    std::tm* p_gmt_tm = gmtime(&time_tt);
    if (p_gmt_tm) {
        gmt_tm_struct = *p_gmt_tm;
    } else {
        return "Invalid Timestamp (gmtime failed)";
    }
#endif
    char buffer[80];
    if (std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S UTC", &gmt_tm_struct) == 0) {
        return "Timestamp Formatting Error (strftime)";
    }
    return string(buffer);
}

void export_to_html(
    const string& json_file_path,
    const string& html_file_path,
    const string& logo_path_param,
    int target_user_id,
    const string& target_user_name) {

    cout << "INFO: Trying to open JSON for report: [" << json_file_path << "]" << endl;
    ifstream in(json_file_path);
    if (!in.is_open()) {
        cerr << "ERROR: Failed to open JSON file for report: " << json_file_path << endl;
        return;
    }

    json all_donations_data;
    try {
        in >> all_donations_data;
    } catch (json::parse_error& e) {
        cerr << "ERROR: JSON parsing error for report: " << e.what() << " at byte " << e.byte << endl;
        in.close();
        return;
    }
    in.close();

    if (!all_donations_data.is_array()) {
        cerr << "ERROR: JSON data for report is not an array as expected." << endl;
        return;
    }

    json* user_specific_donations_ptr = nullptr;
    int user_donation_count = 0;
    int user_donation_capacity = 0;

    for (const auto& item : all_donations_data) {
        bool match = false;
        if (item.contains("user_id") && item["user_id"].is_number_integer()) {
            if (item["user_id"].get<int>() == target_user_id) {
                match = true;
            }
        } else if (item.contains("user_name") && item["user_name"].is_string() && item["user_name"].get<string>() == target_user_name) {
            if (!item.contains("user_id")) {
                match = true;
            }
        }

        if (match) {
            if (user_donation_count == user_donation_capacity) {
                int new_capacity = (user_donation_capacity == 0) ? 10 : user_donation_capacity * 2;
                json* temp_ptr = new (std::nothrow) json[new_capacity];
                if (temp_ptr == nullptr) {
                    cerr << "ERROR: Memory allocation failed while filtering donations for HTML report." << endl;
                    if (user_specific_donations_ptr != nullptr) {
                        delete[] user_specific_donations_ptr;
                    }
                    return;
                }
                for (int k = 0; k < user_donation_count; ++k) {
                    temp_ptr[k] = user_specific_donations_ptr[k];
                }
                if (user_specific_donations_ptr != nullptr) {
                    delete[] user_specific_donations_ptr;
                }
                user_specific_donations_ptr = temp_ptr;
                user_donation_capacity = new_capacity;
            }
            user_specific_donations_ptr[user_donation_count++] = item;
        }
    }

    string logo_path_for_html = logo_path_param;
    ofstream out(html_file_path);
    if (!out.is_open()) {
        cerr << "ERROR: Failed to create HTML file: " << html_file_path << endl;
        if (user_specific_donations_ptr != nullptr) delete[] user_specific_donations_ptr;
        return;
    }

    out << "<!DOCTYPE html>\n";
    out << "<html lang=\"en\">\n<head>\n";
    out << "  <meta charset=\"UTF-8\">\n";
    out << "  <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n";
    out << "  <title>Donation Report for " << target_user_name << "</title>\n";
    out << "  <link href=\"https://fonts.googleapis.com/css2?family=Inter:wght@300;400;500;600;700&display=swap\" rel=\"stylesheet\">\n";
    out << "  <style>\n";
    out << "    body { font-family: 'Inter', sans-serif; margin: 0; padding: 0; background-color: #f8f9fa; color: #212529; line-height: 1.65; font-size: 16px; }\n";
    out << "    .report-header { background-color: #ffffff; padding: 25px 40px; display: flex; align-items: center; justify-content: center; border-bottom: 1px solid #dee2e6; }\n";
    out << "    .report-header .logo { max-height: 120px; width: auto; }\n";
    out << "    .container { padding: 30px 40px; max-width: 960px; margin: 30px auto; background-color: #ffffff; border-radius: 8px; box-shadow: 0 4px 12px rgba(0,0,0,0.05); }\n";
    out << "    .report-main-title { text-align: center; color: #0056b3; margin-bottom: 15px; font-size: 2.4em; font-weight: 700; }\n";
    out << "    .report-user-name { text-align: center; color: #495057; margin-bottom: 35px; font-size: 1.6em; font-weight: 500; }\n";
    out << "    .donations-section { margin-bottom: 30px; }\n";
    out << "    .donation-card { background-color: #ffffff; border: 1px solid #e9ecef; border-left: 5px solid #007bff; border-radius: 6px; padding: 20px 25px; margin-bottom: 25px; transition: box-shadow 0.3s ease; }\n";
    out << "    .donation-card:hover { box-shadow: 0 6px 18px rgba(0,0,0,0.08); }\n";
    out << "    .donation-card p { margin: 0 0 12px 0; font-size: 1em; color: #495057; display: flex; align-items: center; }\n";
    out << "    .donation-card p strong { color: #343a40; min-width: 140px; display: inline-block; font-weight: 600; margin-right: 10px; }\n";
    out << "    .donation-card p:last-child { margin-bottom: 0; }\n";
    out << "    .no-donations { text-align: center; font-size: 1.1em; color: #6c757d; padding: 40px; background-color: #f1f3f5; border-radius: 6px; }\n";
    out << "    .footer { text-align: center; padding: 30px; margin-top: 40px; font-size: 0.9em; color: #6c757d; border-top: 1px solid #dee2e6; background-color: #ffffff; }\n";
    out << "    @media print { body { background-color: #fff !important; -webkit-print-color-adjust: exact !important; print-color-adjust: exact !important; font-size: 12pt;} .report-header, .footer { box-shadow: none; border-bottom: none; border-top: 1px solid #ccc !important; } .container { margin: 0 auto; padding: 10mm 0; box-shadow: none; width: 100% !important; max-width: 100% !important; } .donations-section { page-break-before: auto; box-shadow: none; border: none; margin: 10mm 0;} .donation-card { page-break-inside: avoid; box-shadow: none; border: 1px solid #ccc !important; border-left-width: 3px !important; margin-bottom:5mm !important; } .report-header .logo {max-height: 100px !important;} h1,h2 {page-break-after: avoid;} }\n";
    out << "    @media (max-width: 768px) { .report-header { padding: 20px 15px; } .report-header .logo { max-height: 100px; } .container { padding: 20px 15px; margin-top:15px; } .report-main-title { font-size: 2em; } .report-user-name { font-size: 1.3em; } .donations-section { padding: 20px; } .donation-card p strong { min-width: 110px; } body {font-size:15px;} }\n";
    out << "  </style>\n";
    out << "</head>\n<body>\n";

    out << "  <div class=\"report-header\">\n";
    out << "    <img src=\"" << logo_path_for_html << "\" alt=\"University Logo\" class=\"logo\" onerror=\"this.style.display='none'; console.error('Logo not found at " << logo_path_for_html << "')\">\n";
    out << "  </div>\n";

    out << "  <div class=\"container\">\n";
    out << "    <h1 class=\"report-main-title\">Donation Report</h1>\n";
    out << "    <h2 class=\"report-user-name\">For: " << target_user_name << " (User ID: " << target_user_id << ")</h2>\n";

    if (user_donation_count == 0) {
        out << "    <div class=\"donations-section\">\n";
        out << "      <p class=\"no-donations\">No donations found for this user.</p>\n";
        out << "    </div>\n";
    } else {
        out << "    <div class=\"donations-section\">\n";
        for (int i = 0; i < user_donation_count; ++i) {
            const auto& item = user_specific_donations_ptr[i];
            out << "      <div class=\"donation-card\">\n";
            out << "        <p><strong>Donation ID:</strong> <span>" << (item.contains("donation_id") && item["donation_id"].is_string() ? item["donation_id"].get<string>() : "N/A") << "</span></p>\n";
            out << "        <p><strong>Charity:</strong> <span>" << (item.contains("charity_name") && item["charity_name"].is_string() ? item["charity_name"].get<string>() : "N/A") << "</span></p>\n";
            out << "        <p><strong>Charity ID:</strong> <span>" << (item.contains("charity_id") && item["charity_id"].is_number_integer() ? to_string(item["charity_id"].get<int>()) : "N/A") << "</span></p>\n";

            string amount_str = "N/A";
            if (item.contains("donation_amount")) {
                if (item["donation_amount"].is_number()) {
                    double amount_val = item["donation_amount"].get<double>();
                    ostringstream oss;
                    oss << fixed << setprecision(2) << amount_val;
                    amount_str = "$" + oss.str();
                } else if (item["donation_amount"].is_string()) {
                    try {
                        double amount_val = stod(item["donation_amount"].get<string>());
                        ostringstream oss;
                        oss << fixed << setprecision(2) << amount_val;
                        amount_str = "$" + oss.str();
                    } catch (const std::exception&) {
                        amount_str = item["donation_amount"].get<string>();
                    }
                }
            }
            out << "        <p><strong>Amount:</strong> <span>" << amount_str << "</span></p>\n";

            string formatted_time_utc = "N/A";
            if (item.contains("donation_timestamp") && item["donation_timestamp"].is_number()) {
                formatted_time_utc = format_timestamp(item["donation_timestamp"].get<long long>());
            }
            out << "        <p><strong>Date (UTC):</strong> <span>" << formatted_time_utc << "</span></p>\n";

            if (item.contains("d") && item["d"].is_object()) {
                out << "        <p><strong>Recorded At (Local):</strong> <span>"
                    << item["d"].value("date", "N/A") << " "
                    << item["d"].value("time", "N/A") << "</span></p>\n";
            }

            if (item.contains("message") && item["message"].is_string() && !item["message"].get<string>().empty()){
                 out << "        <p><strong>Message:</strong> <span>" << item["message"].get<string>() << "</span></p>\n";
            }

            out << "      </div>\n";
        }
        out << "    </div>\n";
    }

    out << "  </div>\n";
    out << "  <div class=\"footer\">\n";
    std::time_t now_time_t_footer = std::time(nullptr);
    char time_buf_footer[100];
    std::tm ltm_struct_footer = {};

#ifdef _MSC_VER
    if (localtime_s(<m_struct_footer, &now_time_t_footer) != 0) {
        cerr << "Warning: localtime_s failed in footer." << endl;
        ltm_struct_footer.tm_year = 124;
        ltm_struct_footer.tm_mon = 0;
        ltm_struct_footer.tm_mday = 1;
    }
#else
    std::tm* p_ltm_footer = localtime(&now_time_t_footer);
    if (p_ltm_footer) {
        ltm_struct_footer = *p_ltm_footer;
    } else {
        cerr << "Warning: localtime failed in footer." << endl;
        ltm_struct_footer.tm_year = 124;
        ltm_struct_footer.tm_mon = 0;
        ltm_struct_footer.tm_mday = 1;
    }
#endif

    if (std::strftime(time_buf_footer, sizeof(time_buf_footer), "%Y-%m-%d %H:%M:%S", &ltm_struct_footer) != 0) {
         out << "    <p>Report Generated on: " << time_buf_footer << " (Local)</p>\n";
    } else {
         out << "    <p>Report Generated on: Error formatting date/time</p>\n";
    }
    out << "    <p>© " << (ltm_struct_footer.tm_year + 1900) << " Charlie Haddad - UA. All rights reserved.</p>\n";
    out << "  </div>\n";

    out << "</body>\n</html>\n";
    out.close();
    if (out.fail()) {
        cerr << "ERROR: Failed to write all data to HTML file: " << html_file_path << endl;
    } else {
        cout << "INFO: HTML file generated for user " << target_user_name << ": " << html_file_path << endl;
    }

    if (user_specific_donations_ptr != nullptr) {
        delete[] user_specific_donations_ptr;
    }
}

bool convert_html_to_pdf(const string& html_file_path, const string& pdf_file_path) {
    string command = "wkhtmltopdf";
    command += " --page-size A4";
    command += " --margin-top 15mm --margin-bottom 15mm --margin-left 10mm --margin-right 10mm";
    command += " --enable-local-file-access";
    command += " --print-media-type";
    command += " --disable-smart-shrinking";
    command += " --zoom 1";
    command += " --footer-center \"Page [page] of [topage]\"";
    command += " --footer-font-size 8";
    command += " \"" + html_file_path + "\" \"" + pdf_file_path + "\"";

    cout << "INFO: Executing PDF conversion command: [" << command << "]" << endl;
    int result = system(command.c_str());

    if (result == 0) {
        cout << "INFO: PDF conversion command executed successfully." << endl;
        return true;
    } else {
        cerr << "ERROR: PDF conversion command failed. Exit code: " << result << endl;
        cerr << "Ensure wkhtmltopdf is installed and in your system's PATH." << endl;
        cerr << "Check wkhtmltopdf output/errors if any are printed to console by the command itself." << endl;
        return false;
    }
}