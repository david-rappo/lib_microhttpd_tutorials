#pragma once

// C++ Standard Library
#include <cstddef>
#include <string>

// Guidelines Support Library
#include <gsl/gsl>

namespace Jade
{

namespace Page
{

const std::string name_field{"name"};

const std::size_t maximum_name_size{32};

const std::string ask_page_content{"<html><body>Enter name<br>"
    "<form action=\"/namepost\" method=\"post\">"
    "<input name=\"name\" type=\"text\">"
    "<input type=\"submit\" value=\" Send \"></form>"
    "</body></html>"};

const std::string error_page_content{"<html><body>Error</body></html>"};

std::string create_greeting_page_content(std::string_view name);

} // Page

} // Jade