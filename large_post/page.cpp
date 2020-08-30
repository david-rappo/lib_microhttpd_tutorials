// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "page.h"

// C++ Standard Library
#include <sstream>

namespace Jade
{

namespace Page
{

std::string create_ask_page_content(std::size_t client_count)
{
    std::ostringstream oss;
    oss << "<html><body>";
    oss << "Choose file to upload<br>";
    oss << "There are " << client_count << " clients uploading at the moment<br>";
    oss << "<form action=\"/filepost\" method=\"post\" enctype=\"multipart/form-data\">";
    oss << "<input name=\"file\" type=\"file\">";
    oss << "<input type=\"submit\" value=\"send\"></form>";
    oss << "</body></html>";
    return oss.str();
}

std::string create_file_exists_content(std::experimental::string_view file_name)
{
    std::ostringstream oss;
    oss << "<html><body>";
    oss << "The file \"" << file_name << "\" already exists.";
    oss << "</body></html>";
    return oss.str();
}

} // Page

} // Jade