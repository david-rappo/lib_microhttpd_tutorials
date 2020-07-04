// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

// C++ Standard Library
#include <chrono>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <stdexcept>

// Boost
#include <boost/algorithm/string.hpp>

// System
#include <arpa/inet.h>

#include <fcntl.h>

#include <netinet/in.h>

#include <sys/select.h>
#include <sys/socket.h> // Defines sockaddr
#include <sys/types.h>

// libmicrohttpd
#include <microhttpd.h>

// Guidelines Support Library
#include <gsl/gsl>

// response_headers program
#include "file_utility.h"

int answer_to_connection(void *cls,
    struct MHD_Connection *connection,
    const char *url,
    const char *method,
    const char *version,
    const char *upload_data,
    size_t *upload_data_size,
    void **con_cls);

std::string create_error_page();

// Note: decltype(MHD_destroy_response) returns the type of the function MHD_destroy_response. The
//       '*' indicates that the type of the std::unique_ptr template paramter _Dp is a pointer.
std::unique_ptr<MHD_Response, decltype(MHD_destroy_response)*> create_response();

std::unique_ptr<MHD_Response, decltype(MHD_destroy_response)*> create_error_response();

int main(int argc, char **argv)
{
    constexpr uint16_t port{8888};
    // Flag MHD_USE_PEDANTIC_CHECKS is deprecated. It was replaced with MHD_OPTION_STRICT_FOR_CLIENT.
    // However, MHD_OPTION_STRICT_FOR_CLIENT does not seem to work.
    const auto flags = MHD_USE_AUTO|MHD_USE_INTERNAL_POLLING_THREAD|MHD_USE_PEDANTIC_CHECKS;
    auto daemon = MHD_start_daemon(flags,
        port,
        nullptr,
        nullptr,
        &answer_to_connection,
        nullptr,
        MHD_OPTION_END);
    if (nullptr == daemon)
    {
        return 0;
    }

    std::cout << "Starting...\n";
    std::unique_ptr<MHD_Daemon, decltype(MHD_stop_daemon)*> up_daemon{daemon, MHD_stop_daemon};
    // The program will finish when the user enters a character. If the call to getchar() is omitted
    // then the cleanup code for the sever daemon will run immediately. There are better ways to do
    // this of course but it is fine for a simple example program such as this one.
    getchar();
    return 0;
}

std::string create_error_page()
{
    std::ostringstream oss;
    oss << "<html>";
    oss << "<body>";
    oss << "<h1>MHD_HTTP_INTERNAL_SERVER_ERROR</h1>";
    oss << "<p>An internal server error occurred</p>";
    oss << "</body>";
    oss << "</html>";
    return oss.str();
}

// open(2) - Linux man page: https://linux.die.net/man/2/open
std::unique_ptr<MHD_Response, decltype(MHD_destroy_response)*> create_response()
{
    using namespace std::string_literals;
    
    const auto file_name{"picture.png"s};
    auto file_descriptor = open(file_name.c_str(), O_RDONLY);
    if (-1 == file_descriptor)
    {
        return create_error_response();
    }

    std::unique_ptr<int, decltype(File_utility::close_file_descriptor)*> up_close_file_descriptor(&file_descriptor,
        File_utility::close_file_descriptor);
    const auto file_size = File_utility::get_file_size(file_descriptor);
    if (0 == file_size)
    {
        return create_error_response();
    }
    
    auto response = MHD_create_response_from_fd_at_offset64(file_size, file_descriptor, 0);
    if (nullptr == response)
    {
        throw std::runtime_error{"Failed to create MHD_Response"};
    }

    std::unique_ptr<MHD_Response, decltype(MHD_destroy_response)*> up_response{response, MHD_destroy_response};
    // The instance of MHD_Response returned by MHD_create_response_from_fd_at_offset64 will close
    // file_descriptor.
    file_descriptor = *up_close_file_descriptor.release();
    const auto content{"image/png"s};
    const auto result = MHD_add_response_header(response, MHD_HTTP_HEADER_CONTENT_TYPE, content.c_str());
    if (MHD_NO == result)
    {
        std::ostringstream oss;
        oss << "Failed to add \"" << MHD_HTTP_HEADER_CONTENT_TYPE << "\" header to MHD_Response";
        throw std::runtime_error{oss.str()};
    }

    std::cout << "Returning MHD_Response created by MHD_create_response_from_fd_at_offset64\n";
    return up_response;
}

std::unique_ptr<MHD_Response, decltype(MHD_destroy_response)*> create_error_response()
{
    const auto page = create_error_page();
    auto response = MHD_create_response_from_buffer(page.length(),
        const_cast<char*>(page.c_str()),
        MHD_RESPMEM_MUST_COPY);
    if (nullptr == response)
    {
        throw std::runtime_error{"Failed to create MHD_Response"};
    }
    
    std::unique_ptr<MHD_Response, decltype(MHD_destroy_response)*> up_response{response,
        MHD_destroy_response};
    return up_response;
}

int answer_to_connection(void *cls,
    struct MHD_Connection *connection,
    const char *url,
    const char *method,
    const char *version,
    const char *upload_data,
    size_t *upload_data_size,
    void **con_cls)
{
    auto method_string = std::string(method);
    if ("GET" != method_string)
    {
        return MHD_NO;
    }

    auto up_response = create_response();
    return MHD_queue_response(connection, MHD_HTTP_OK, up_response.get());
}