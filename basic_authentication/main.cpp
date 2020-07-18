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
#include <string_view>
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

#include <unistd.h>

// libmicrohttpd
#include <microhttpd.h>

// Guidelines Support Library
#include <gsl/gsl>

int answer_to_connection(void *cls,
    struct MHD_Connection *connection,
    const char *url,
    const char *method,
    const char *version,
    const char *upload_data,
    size_t *upload_data_size,
    void **con_cls);

std::string create_page(std::string_view title, std::string_view text);

// Note: decltype(MHD_destroy_response) returns the type of the function MHD_destroy_response. The
//       '*' indicates that the type of the std::unique_ptr template paramter _Dp is a pointer.
std::unique_ptr<MHD_Response, decltype(MHD_destroy_response)*> create_response(
    std::string_view title,
    std::string_view text);

int main(int argc, char **argv)
{
    constexpr uint16_t port{8888};
    // Flag MHD_USE_PEDANTIC_CHECKS is deprecated. It was replaced with MHD_OPTION_STRICT_FOR_CLIENT.
    // However, MHD_OPTION_STRICT_FOR_CLIENT does not seem to work.
    const auto flags = MHD_USE_AUTO|
        MHD_USE_INTERNAL_POLLING_THREAD|
        MHD_USE_PEDANTIC_CHECKS;
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

std::string create_page(std::string_view title, std::string_view text)
{
    std::ostringstream oss;
    oss << "<html>";
    oss << "<body>";
    oss << "<h1>" << title.data() << "</h1>";
    oss << "<p>" << text.data() << "</p>";
    oss << "</body>";
    oss << "</html>";
    return oss.str();
}

std::unique_ptr<MHD_Response, decltype(MHD_destroy_response)*> create_response(
    std::string_view title,
    std::string_view text)
{
    const auto page = create_page(title, text);
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
    using namespace std::string_literals;
    
    auto method_string = std::string(method);
    if ("GET" != method_string)
    {
        return MHD_NO;
    }

    assert(nullptr != con_cls);
    auto &extra_connection_data = *con_cls;
    if (nullptr == extra_connection_data)
    {
        // Do not do anything the very first time that answer_to_connection is called. On subsequent
        // calls to answer_to_connection extra_connection_data will not be nullptr. The data that
        // extra_connection_data points to is not important for the purposes of this program.
        
        std::cout << "extra_connection_data is NULL. Assigning connection\n";
        extra_connection_data = connection;
        return MHD_YES;
    }
    
    char *password{nullptr};
    std::cout << "Attempting to fetch User Name and Password\n";
    auto user_name = MHD_basic_auth_get_username_password(connection, &password);
    std::unique_ptr<char, decltype(free)*> up_password(password, free);
    std::unique_ptr<char, decltype(free)*> up_user_name(user_name, free);
    const auto realm = "basic-auth-realm"s;
    if (nullptr == user_name)
    {
        const auto title = "Authentication Failure"s;
        const auto text = "User Name not found"s;
        auto up_response = create_response(title, text);
        return MHD_queue_basic_auth_fail_response(connection, realm.c_str(), up_response.get());
    }

    const auto expected_user_name = "root"s;
    const auto expected_password = "password"s;
    if (expected_user_name != user_name)
    {
        const auto title = "Authentication Failure"s;
        const auto text = "Invalid User Name"s;
        auto up_response = create_response(title, text);
        return MHD_queue_basic_auth_fail_response(connection, realm.c_str(), up_response.get());
    }

    if (expected_password != password)
    {
        const auto title = "Authentication Failure"s;
        const auto text = "Invalid Password"s;
        auto up_response = create_response(title, text);
        return MHD_queue_basic_auth_fail_response(connection, realm.c_str(), up_response.get());
    }

    const auto title = "Main Page"s;
    const auto text = "Hello World"s;
    auto up_response = create_response(title, text);
    return MHD_queue_response(connection, MHD_HTTP_OK, up_response.get());
}