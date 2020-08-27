#pragma once

// C++ Standard Library
#include <experimental/string_view>
#include <cstddef>
#include <string>

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

const std::string busy_page_content{"<html><body>The server is busy.</body></html>"};

std::string create_greeting_page_content(std::experimental::string_view name);

} // Page

} // Jade