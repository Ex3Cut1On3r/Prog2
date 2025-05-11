#ifndef EXPORTTOPDF_H
#define EXPORTTOPDF_H
#include <string>
void export_to_html(const std::string& json_file_path,
const std::string& html_file_path,const std::string& logo_path);
bool convert_html_to_pdf(const std::string& html_file_path,
const std::string& pdf_file_path);
#endif