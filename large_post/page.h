#pragma once

// C++ Standard Library
#include <experimental/string_view>
#include <cstddef>
#include <string>

namespace Jade
{

namespace Page
{

const std::string file_field{"file"};

const std::string complete_page_content{"<html><body>Upload completed</body></html>"};

const std::string error_page_content{"<html><body>Error</body></html>"};

const std::string busy_page_content{"<html><body>The server is busy.</body></html>"};

const std::string post_process_error_content{
    "<html><head><title>Error</title></head><body>Error processing POST data</body></html>"};

const std::string file_input_output_error_page_content{"<html><body>File Input Output error</body></html>"};

std::string create_ask_page_content(std::size_t client_count);

std::string create_file_exists_content(std::experimental::string_view file_name);

} // Page

} // Jade