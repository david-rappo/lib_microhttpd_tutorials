// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "page.h"

// C++ Standard Library
#include <sstream>

namespace Jade
{

namespace Page
{

std::string create_greeting_page_content(std::experimental::string_view name)
{
    std::ostringstream oss;
    oss << "<html><body><h1>Welcome ";
    oss << name.data();
    oss << "</center></h1></body></html>";
    return oss.str();
}

} // Page

} // Jade