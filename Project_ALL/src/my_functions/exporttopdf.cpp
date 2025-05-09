// File: Project_ALL/src/my_functions/exporttopdf.cpp

#include "structsandfunctions/exporttopdf.h"
#include "nlohman/json.hpp" // Ensure this path is correct for your project setup

#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>    // For std::system
#include <iomanip>    // For std::fixed, std::setprecision
#include <sstream>    // For std::ostringstream
#include <map>        // For std::map (to group donations by user)
#include <algorithm>  // For std::replace (if needed for paths)
#include <chrono>     // For time formatting
#include <ctime>      // For time formatting

using json = nlohmann::json;

// Helper function to format Unix timestamp to a human-readable string (UTC)
std::string format_timestamp(long long unix_timestamp) {
    if (unix_timestamp <= 0) return "N/A";

    auto tp = std::chrono::system_clock::from_time_t(static_cast<std::time_t>(unix_timestamp));
    std::time_t time_tt = std::chrono::system_clock::to_time_t(tp);

    std::tm gmt_tm_struct;
#ifdef _MSC_VER
    gmtime_s(&gmt_tm_struct, &time_tt);
#else
    std::tm* p_gmt_tm = std::gmtime(&time_tt);
    if (p_gmt_tm) {
        gmt_tm_struct = *p_gmt_tm;
    } else {
        return "Invalid Timestamp";
    }
#endif

    char buffer[80];
    if (std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S UTC", &gmt_tm_struct) == 0) {
        return "Formatting Error";
    }
    return std::string(buffer);
}


// Updated export_to_html function
void export_to_html(const std::string& json_file_path, const std::string& html_file_path, const std::string& logo_path_param) {
    std::cout << "DEBUG: export_to_html trying to open JSON: [" << json_file_path << "]" << std::endl;

    std::ifstream in(json_file_path);
    if (!in.is_open()) {
        std::cerr << "ERROR: Failed to open JSON file: " << json_file_path << std::endl;
        return;
    }

    json data;
    try {
        in >> data;
    } catch (json::parse_error& e) {
        std::cerr << "ERROR: JSON parsing error: " << e.what() << " at byte " << e.byte << std::endl;
        in.close();
        return;
    }
    in.close();

    if (!data.is_array()) {
        std::cerr << "ERROR: JSON data is not an array as expected." << std::endl;
        return;
    }

    // Group donations by user
    std::map<std::string, std::vector<json>> donations_by_user;
    if (data.is_array()) {
        for (const auto& item : data) {
            std::string userName = "Anonymous Donor"; // Default if user_name is missing or not a string
            if (item.contains("user_name") && item["user_name"].is_string()) {
                userName = item["user_name"].get<std::string>();
                if (userName.empty()) userName = "Anonymous Donor (empty name)";
            }
            donations_by_user[userName].push_back(item);
        }
    }

    std::string logo_path_for_html = logo_path_param;
    // For local files in <img> src, wkhtmltopdf with --enable-local-file-access
    // usually handles absolute paths well. If issues, prepend "file:///"
    // For Windows paths with backslashes, they might need to be converted to forward slashes for "file:///"
    // e.g., std::replace(logo_path_for_html.begin(), logo_path_for_html.end(), '\\', '/');
    // logo_path_for_html = "file:///" + logo_path_for_html;

    std::ofstream out(html_file_path);
    if (!out.is_open()) {
        std::cerr << "ERROR: Failed to create HTML file: " << html_file_path << std::endl;
        return;
    }

    out << "<!DOCTYPE html>\n";
    out << "<html lang=\"en\">\n<head>\n";
    out << "  <meta charset=\"UTF-8\">\n";
    out << "  <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n";
    out << "  <title>Donations Report</title>\n";
    out << "  <link href=\"https://fonts.googleapis.com/css2?family=Inter:wght@300;400;500;600;700&display=swap\" rel=\"stylesheet\">\n"; // Modern Font
    out << "  <style>\n";
    out << "    body {\n";
    out << "        font-family: 'Inter', sans-serif;\n";
    out << "        margin: 0;\n";
    out << "        padding: 0;\n";
    out << "        background-color: #f4f7f9;\n"; // Lighter, softer background
    out << "        color: #333;\n";
    out << "        line-height: 1.6;\n";
    out << "    }\n";
    out << "    .report-header {\n";
    out << "        background-color: #2c3e50;\n"; // Dark slate blue
    out << "        color: #ffffff;\n";
    out << "        padding: 25px 40px;\n";
    out << "        display: flex;\n";
    out << "        align-items: center;\n";
    out << "        justify-content: space-between;\n";
    out << "        box-shadow: 0 2px 5px rgba(0,0,0,0.1);\n";
    out << "    }\n";
    out << "    .report-header h1 {\n";
    out << "        margin: 0;\n";
    out << "        font-size: 1.7em;\n";
    out << "        font-weight: 600;\n";
    out << "    }\n";
    out << "    .report-header .logo {\n";
    out << "        max-height: 55px;\n";
    out << "        width: auto;\n";
    out << "    }\n";
    out << "    .container {\n";
    out << "        padding: 30px 40px;\n";
    out << "        max-width: 900px;\n"; // Content width
    out << "        margin: 20px auto;\n";
    out << "    }\n";
    out << "    .report-main-title {\n";
    out << "        text-align: center;\n";
    out << "        color: #34495e;\n";
    out << "        margin-bottom: 40px;\n";
    out << "        font-size: 2.5em;\n";
    out << "        font-weight: 700;\n";
    out << "    }\n";
    // User Section - for grouping donations by user and page breaks
    out << "    .user-section {\n";
    out << "        margin-bottom: 40px;\n";
    out << "        background-color: #ffffff;\n";
    out << "        border-radius: 8px;\n";
    out << "        box-shadow: 0 5px 15px rgba(0,0,0,0.07);\n";
    out << "        padding: 25px 30px;\n";
    out << "    }\n";
    out << "    .user-section h2 {\n"; // User name as section title
    out << "        font-size: 1.8em;\n";
    out << "        color: #2980b9;\n"; // A nice blue
    out << "        margin-top: 0;\n";
    out << "        margin-bottom: 25px;\n";
    out << "        padding-bottom: 10px;\n";
    out << "        border-bottom: 2px solid #ecf0f1;\n";
    out << "    }\n";
    // Donation Card - for individual donations
    out << "    .donation-card {\n";
    out << "        background-color: #fdfdfd;\n"; // Slightly off-white
    out << "        border: 1px solid #e0e6ed;\n";
    out << "        border-radius: 6px;\n";
    out << "        padding: 20px;\n";
    out << "        margin-bottom: 20px;\n";
    // out << "        box-shadow: 0 2px 8px rgba(0,0,0,0.05);\n"; // Subtle shadow
    out << "    }\n";
    out << "    .donation-card p {\n";
    out << "        margin: 0 0 10px 0;\n";
    out << "        font-size: 0.95em;\n";
    out << "        color: #555;\n";
    out << "    }\n";
    out << "    .donation-card p strong {\n";
    out << "        color: #333;\n";
    out << "        min-width: 120px; /* Align labels nicely */\n";
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
    // Print-specific styles for PDF generation
    out << "    @media print {\n";
    out << "        body { background-color: #fff; -webkit-print-color-adjust: exact; print-color-adjust: exact;}\n"; // Ensure backgrounds print
    out << "        .report-header { box-shadow: none; }\n"; // Remove shadow in print
    out << "        .container { margin: 0 auto; padding: 20px 0; box-shadow: none; }\n";
    out << "        .user-section {\n";
    out << "            page-break-before: always; /* Each user's section on a new page */\n";
    out << "            box-shadow: none;\n";
    out << "            border: 1px solid #ccc; /* Add border for print if desired */\n";
    out << "            margin: 20px 0;\n"; // Adjust margin for print
    out << "        }\n";
    out << "        .user-section:first-of-type {\n"; // Changed from :first-child to handle dynamic first element
    out << "            page-break-before: auto; /* No page break before the very first section */\n";
    out << "        }\n";
    out << "        .donation-card {\n";
    out << "            page-break-inside: avoid; /* Try not to break a card across pages */\n";
    out << "            box-shadow: none;\n";
    out << "            border: 1px solid #ddd;\n";
    out << "        }\n";
    out << "        .footer { border-top: 1px solid #ccc; }\n";
    out << "    }\n";
    // Responsive adjustments
    out << "    @media (max-width: 768px) {\n";
    out << "        .report-header { flex-direction: column; text-align: center; padding: 20px; }\n";
    out << "        .report-header h1 { margin-bottom: 15px; font-size: 1.4em; }\n";
    out << "        .report-header .logo { max-height: 45px; margin-top: 10px; }\n";
    out << "        .container { padding: 20px 15px; }\n";
    out << "        .report-main-title { font-size: 2em; }\n";
    out << "        .user-section { padding: 20px; }\n";
    out << "        .user-section h2 { font-size: 1.5em; }\n";
    out << "        .donation-card p strong { min-width: 100px; }\n";
    out << "    }\n";
    out << "  </style>\n";
    out << "</head>\n<body>\n";

    out << "  <div class=\"report-header\">\n";
    out << "    <h1>Charlie Haddad Programming2 project UA</h1>\n";
    out << "    <img src=\"" << logo_path_for_html << "\" alt=\"University Logo\" class=\"logo\" onerror=\"this.style.display='none'; console.error('Logo not found at " << logo_path_for_html << "')\">\n";
    out << "  </div>\n";

    out << "  <div class=\"container\">\n";
    out << "    <h1 class=\"report-main-title\">Donation Report</h1>\n";

    if (donations_by_user.empty()) {
        out << "    <div class=\"user-section\">\n"; // Use user-section for consistent styling
        out << "      <p class=\"no-donations\">No donations found in the report.</p>\n";
        out << "    </div>\n";
    } else {
        bool first_user_section = true;
        for (const auto& pair : donations_by_user) {
            const std::string& userName = pair.first;
            const std::vector<json>& user_donations = pair.second;

            out << "    <div class=\"user-section\"" << (first_user_section ? " style=\"page-break-before: auto;\"" : "") << ">\n"; // Inline style for first to override CSS for print
            out << "      <h2>" << userName << "</h2>\n";

            if (user_donations.empty()) {
                 out << "<p>No donations recorded for this user.</p>\n";
            } else {
                for (const auto& item : user_donations) {
                    out << "      <div class=\"donation-card\">\n";
                    out << "        <p><strong>Charity:</strong> " << (item.contains("charity_name") && item["charity_name"].is_string() ? item["charity_name"].get<std::string>() : "N/A") << "</p>\n";

                    std::string amount_str = "N/A";
                    if (item.contains("donation_amount")) {
                        if (item["donation_amount"].is_number()) {
                            double amount_val = item["donation_amount"].get<double>();
                            std::ostringstream oss;
                            oss << std::fixed << std::setprecision(2) << amount_val;
                            amount_str = "$" + oss.str();
                        } else if (item["donation_amount"].is_string()) {
                             // Attempt to convert string to double for consistent formatting, or use as is
                            try {
                                double amount_val = std::stod(item["donation_amount"].get<std::string>());
                                std::ostringstream oss;
                                oss << std::fixed << std::setprecision(2) << amount_val;
                                amount_str = "$" + oss.str();
                            } catch (const std::exception&) {
                                amount_str = item["donation_amount"].get<std::string>(); // Use as is if not a number
                            }
                        }
                    }
                    out << "        <p><strong>Amount:</strong> " << amount_str << "</p>\n";

                    std::string formatted_time = "N/A";
                    if (item.contains("donation_timestamp") && item["donation_timestamp"].is_number()) {
                        formatted_time = format_timestamp(item["donation_timestamp"].get<long long>());
                    }
                    out << "        <p><strong>Date:</strong> " << formatted_time << "</p>\n";
                    out << "      </div>\n"; // End donation-card
                }
            }
            out << "    </div>\n"; // End user-section
            first_user_section = false;
        }
    }

    out << "  </div>\n"; // End container

    // Footer
    out << "  <div class=\"footer\">\n";
    time_t now = std::time(nullptr);
    char time_buf[100];
    std::strftime(time_buf, sizeof(time_buf), "%Y-%m-%d %H:%M:%S %Z", std::localtime(&now));
    out << "    <p>Report Generated on: " << time_buf << "</p>\n";
    out << "    <p>© " << format_timestamp(std::time(nullptr)).substr(0,4) << " Charlie Haddad - UA. All rights reserved.</p>\n"; // Dynamic year
    out << "  </div>\n";

    out << "</body>\n</html>";
    out.close();
    std::cout << "INFO: HTML file generated: " << html_file_path << std::endl;
}


// The convert_html_to_pdf function remains largely the same.
// Ensure wkhtmltopdf is installed and in PATH.
// --enable-local-file-access is important for local images/resources.
bool convert_html_to_pdf(const std::string& html_file_path, const std::string& pdf_file_path) {
    std::string command = "wkhtmltopdf";
    command += " --page-size A4";
    command += " --margin-top 15mm --margin-bottom 15mm --margin-left 10mm --margin-right 10mm";
    command += " --enable-local-file-access"; // Crucial for local images like logo
    command += " --print-media-type";         // Use @media print styles
    command += " --disable-smart-shrinking";  // Can help with consistent sizing
    command += " --zoom 1";                   // Default zoom
    command += " --footer-center \"Page [page] of [topage]\""; // Example footer
    command += " --footer-font-size 8";
    command += " \"" + html_file_path + "\" \"" + pdf_file_path + "\"";

    std::cout << "INFO: Executing command: [" << command << "]" << std::endl;
    int result = std::system(command.c_str());

    if (result == 0) {
        std::cout << "INFO: PDF conversion command executed successfully." << std::endl;
        return true;
    } else {
        std::cerr << "ERROR: PDF conversion command failed. Exit code from system(): " << result << std::endl;
        std::cerr << "Ensure wkhtmltopdf is installed and in your system's PATH." << std::endl;
        std::cerr << "Check wkhtmltopdf output/errors if any are printed to console by the command itself." << std::endl;
        return false;
    }
}