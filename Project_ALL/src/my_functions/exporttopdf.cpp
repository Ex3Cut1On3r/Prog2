#include "structsandfunctions/exporttopdf.h"
#include "nlohman/json.hpp"
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>
#include <iomanip>
#include <sstream>
#include <map>
#include <chrono>
#include <ctime>
using namespace std;
using json = nlohmann::json;
string format_timestamp(long long unix_timestamp) {
    if (unix_timestamp <= 0) return "N/A";
    auto tp = chrono::system_clock::from_time_t(static_cast<time_t>(unix_timestamp));
    time_t time_tt = chrono::system_clock::to_time_t(tp);
    tm gmt_tm_struct;
#ifdef _MSC_VER
    gmtime_s(&gmt_tm_struct, &time_tt);
#else
    tm* p_gmt_tm = gmtime(&time_tt);
    if (p_gmt_tm) {
        gmt_tm_struct = *p_gmt_tm;
    } else {
        return "Invalid Timestamp";
    }
#endif
    char buffer[80];
    if (strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S UTC", &gmt_tm_struct) == 0) {
        return "Formatting Error";
    }
    return string(buffer);
}
void export_to_html(const string& json_file_path, const string& html_file_path, const string& logo_path_param) {
    cout << "Trying to open JSON: [" << json_file_path << "]" << endl;
    ifstream in(json_file_path);
    if (!in.is_open()) {
        cout << "Failed to open JSON file: " << json_file_path << endl;
        return;
    }
    json data;
    try {
        in >> data;
    } catch (json::parse_error& e) {
        cout << "JSON parsing error: " << e.what() << " at byte " << e.byte << endl;
        in.close();
        return;
    }
    in.close();
    if (!data.is_array()) {
        cout << "JSON data is not an array as expected." << endl;
        return;
    }
    map<string, vector<json>> donations_by_user;
    if (data.is_array()) {
        for (const auto& item : data) {
            string userName = "Anonymous Donor";
            if (item.contains("user_name") && item["user_name"].is_string()) {
                userName = item["user_name"].get<string>();
                if (userName.empty()) userName = "Anonymous Donor (empty name)";
            }
            donations_by_user[userName].push_back(item);
        }
    }

    string logo_path_for_html = logo_path_param;
    ofstream out(html_file_path);
    if (!out.is_open()) {
        cout << "Failed to create HTML file: " << html_file_path << endl;
        return;
    }
    out << "<!DOCTYPE html>\n";
    out << "<html lang=\"en\">\n<head>\n";
    out << "  <meta charset=\"UTF-8\">\n";
    out << "  <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n";
    out << "  <title>Donations Report</title>\n";
    out << "  <link href=\"https://fonts.googleapis.com/css2?family=Inter:wght@300;400;500;600;700&display=swap\" rel=\"stylesheet\">\n";
    out << "  <style>\n";
    out << "    body {\n";
    out << "        font-family: 'Inter', sans-serif;\n";
    out << "        margin: 0;\n";
    out << "        padding: 0;\n";
    out << "        background-color: #f4f7f9;\n";
    out << "        color: #333;\n";
    out << "        line-height: 1.6;\n";
    out << "    }\n";
    out << "    .report-header {\n";
    out << "        background-color: #FFFFFF;\n";
    out << "        padding: 20px 40px;\n";
    out << "        display: flex;\n";
    out << "        align-items: center;\n";
    out << "        justify-content: center;\n";
    out << "        box-shadow: 0 2px 5px rgba(0,0,0,0.1);\n";
    out << "    }\n";
    out << "    .report-header .logo {\n";
    out << "        max-height: 150px;\n";
    out << "        width: auto;\n";
    out << "    }\n";
    out << "    .container {\n";
    out << "        padding: 30px 40px;\n";
    out << "        max-width: 900px;\n";
    out << "        margin: 20px auto;\n";
    out << "    }\n";
    out << "    .report-main-title {\n";
    out << "        text-align: center;\n";
    out << "        color: #34495e;\n";
    out << "        margin-bottom: 40px;\n";
    out << "        font-size: 2.5em;\n";
    out << "        font-weight: 700;\n";
    out << "    }\n";
    out << "    .user-section {\n";
    out << "        margin-bottom: 40px;\n";
    out << "        background-color: #ffffff;\n";
    out << "        border-radius: 8px;\n";
    out << "        box-shadow: 0 5px 15px rgba(0,0,0,0.07);\n";
    out << "        padding: 25px 30px;\n";
    out << "    }\n";
    out << "    .user-section h2 {\n";
    out << "        font-size: 1.8em;\n";
    out << "        color: #2980b9;\n";
    out << "        margin-top: 0;\n";
    out << "        margin-bottom: 25px;\n";
    out << "        padding-bottom: 10px;\n";
    out << "        border-bottom: 2px solid #ecf0f1;\n";
    out << "    }\n";
    out << "    .donation-card {\n";
    out << "        background-color: #fdfdfd;\n";
    out << "        border: 1px solid #e0e6ed;\n";
    out << "        border-radius: 6px;\n";
    out << "        padding: 20px;\n";
    out << "        margin-bottom: 20px;\n";
    out << "    }\n";
    out << "    .donation-card p {\n";
    out << "        margin: 0 0 10px 0;\n";
    out << "        font-size: 0.95em;\n";
    out << "        color: #555;\n";
    out << "    }\n";
    out << "    .donation-card p strong {\n";
    out << "        color: #333;\n";
    out << "        min-width: 120px;\n";
    out << "        display: inline-block;\n";
    out << "        font-weight: 500;\n";
    out << "    }\n";
    out << "    .donation-card p:last-child {\n";
    out << "        margin-bottom: 0;\n";
    out << "    }\n";
    out << "    .no-donations {\n";
    out << "        text-align: center;\n";
    out << "        font-size: 1.1em;\n";
    out << "        color: #7f8c8d;\n";
    out << "        padding: 30px;\n";
    out << "    }\n";
    out << "    .footer {\n";
    out << "        text-align: center;\n";
    out << "        padding: 25px;\n";
    out << "        margin-top: 50px;\n";
    out << "        font-size: 0.9em;\n";
    out << "        color: #888;\n";
    out << "        border-top: 1px solid #e0e6ed;\n";
    out << "    }\n";
    out << "    @media print {\n";
    out << "        body { background-color: #fff; -webkit-print-color-adjust: exact; print-color-adjust: exact;}\n";
    out << "        .report-header { box-shadow: none; }\n";
    out << "        .container { margin: 0 auto; padding: 20px 0; box-shadow: none; }\n";
    out << "        .user-section {\n";
    out << "            page-break-before: always;\n";
    out << "            box-shadow: none;\n";
    out << "            border: 1px solid #ccc;\n";
    out << "            margin: 20px 0;\n";
    out << "        }\n";
    out << "        .user-section:first-of-type {\n";
    out << "            page-break-before: auto;\n";
    out << "        }\n";
    out << "        .donation-card {\n";
    out << "            page-break-inside: avoid;\n";
    out << "            box-shadow: none;\n";
    out << "            border: 1px solid #ddd;\n";
    out << "        }\n";
    out << "        .footer { border-top: 1px solid #ccc; }\n";
    out << "    }\n";
    out << "    @media (max-width: 768px) {\n";
    out << "        .report-header { \n";
    out << "           padding: 15px; \n";
    out << "        }\n";
    out << "        .report-header .logo { \n";
    out << "           max-height: 100px; \n";
    out << "        }\n";
    out << "        .container { padding: 20px 15px; }\n";
    out << "        .report-main-title { font-size: 2em; }\n";
    out << "        .user-section { padding: 20px; }\n";
    out << "        .user-section h2 { font-size: 1.5em; }\n";
    out << "        .donation-card p strong { min-width: 100px; }\n";
    out << "    }\n";
    out << "  </style>\n";
    out << "</head>\n<body>\n";

    out << "  <div class=\"report-header\">\n";
    out << "    <img src=\"" << logo_path_for_html << "\" alt=\"University Logo\" class=\"logo\" onerror=\"this.style.display='none'; console.error('Logo not found at " << logo_path_for_html << "')\">\n";
    out << "  </div>\n";

    out << "  <div class=\"container\">\n";
    out << "    <h1 class=\"report-main-title\">Donation Report</h1>\n";

    if (donations_by_user.empty()) {
        out << "    <div class=\"user-section\">\n";
        out << "      <p class=\"no-donations\">No donations found in the report.</p>\n";
        out << "    </div>\n";
    } else {
        bool first_user_section = true;
        for (const auto& pair : donations_by_user) {
            const string& userName = pair.first;
            const vector<json>& user_donations = pair.second;

            out << "    <div class=\"user-section\"" << (first_user_section ? " style=\"page-break-before: auto;\"" : "") << ">\n";
            out << "      <h2>" << userName << "</h2>\n";

            if (user_donations.empty()) {
                 out << "<p>No donations recorded for this user.</p>\n";
            } else {
                for (const auto& item : user_donations) {
                    out << "      <div class=\"donation-card\">\n";
                    out << "        <p><strong>Charity:</strong> " << (item.contains("charity_name") && item["charity_name"].is_string() ? item["charity_name"].get<string>() : "N/A") << "</p>\n";

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
                            } catch (const exception&) {
                                amount_str = item["donation_amount"].get<string>();
                            }
                        }
                    }
                    out << "        <p><strong>Amount:</strong> " << amount_str << "</p>\n";

                    string formatted_time = "N/A";
                    if (item.contains("donation_timestamp") && item["donation_timestamp"].is_number()) {
                        formatted_time = format_timestamp(item["donation_timestamp"].get<long long>());
                    }
                    out << "        <p><strong>Date:</strong> " << formatted_time << "</p>\n";
                    out << "      </div>\n";
                }
            }
            out << "    </div>\n";
            first_user_section = false;
        }
    }

    out << "  </div>\n";
    out << "  <div class=\"footer\">\n";
    time_t now = time(nullptr);
    char time_buf[100];
    strftime(time_buf, sizeof(time_buf), "%Y-%m-%d %H:%M:%S %Z", localtime(&now));
    out << "    <p>Report Generated on: " << time_buf << "</p>\n";

    tm ltm_struct;
#ifdef _MSC_VER
    localtime_s(<m_struct, &now);
#else
    tm* p_ltm = localtime(&now);
    if (p_ltm) {
        ltm_struct = *p_ltm;
    }
#endif
    out << "    <p>© " << (ltm_struct.tm_year + 1900) << " Charlie Haddad - UA. All rights reserved.</p>\n";
    out << "  </div>\n";

    out << "</body>\n</html>\n";
    out.close();
    cout << "HTML file generated: " << html_file_path << endl;
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

    cout << "Executing command: [" << command << "]" << endl;
    int result = system(command.c_str());

    if (result == 0) {
        cout << "PDF conversion command executed successfully." << endl;
        return true;
    } else {
        cout << "PDF conversion command failed. Exit code from system(): " << result << endl;
        cout << "Ensure wkhtmltopdf is installed and in your system's PATH." << endl;
        cout << "Check wkhtmltopdf output/errors if any are printed to console by the command itself." << endl;
        return false;
    }
}