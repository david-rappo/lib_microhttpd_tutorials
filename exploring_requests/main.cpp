// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

// C++ Standard Library
#include <chrono>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>

// Boost
#include <boost/algorithm/string.hpp>

// System
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/types.h>

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

int print_key(void *cls, enum MHD_ValueKind kind, const char *key, const char *value);

std::string create_index_page(const std::string &url);

std::string create_another_page(const std::string &url);

std::string create_not_found_page(const std::string &url);

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

int print_key(void *cls, enum MHD_ValueKind kind, const char *key, const char *value)
{
    std::cout << key << ": " << value << "\n";
    return MHD_YES;
}

std::string create_index_page(const std::string &url)
{
    std::ostringstream oss;
    oss << "<html>";
    oss << "<body>";
    oss << "<h1>MHD_HTTP_OK</h1>";
    oss << "<p>";
    oss << "URL: " << url;
    oss << "</p>";
    oss << "<p>";
    oss << "<a href=\"another.html\">another.html</a>";
    oss << "</p>";
    oss << "</body>";
    oss << "</html>";
    return oss.str();
}

std::string create_another_page(const std::string &url)
{
    std::ostringstream oss;
    oss << "<html>";
    oss << "<body>";
    oss << "<h1>MHD_HTTP_OK</h1>";
    oss << "<p>";
    oss << "URL: " << url;
    oss << "</p>";
    oss << "</body>";
    oss << "</html>";
    return oss.str();
}

std::string create_not_found_page(const std::string &url)
{
    std::ostringstream oss;
    oss << "<html>";
    oss << "<body>";
    oss << "<h1>MHD_HTTP_NOT_FOUND</h1>";
    oss << "<p>";
    oss << "URL: " << url;
    oss << "</p>";
    oss << "</body>";
    oss << "</html>";
    return oss.str();
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
    std::cout << "New " << method << " request for " << url << " using " << version << "\n";
    MHD_get_connection_values(connection, MHD_HEADER_KIND, print_key, nullptr);
    auto method_string = std::string(method);
    auto url_string = std::string(url);
    boost::algorithm::trim(url_string);
    if ("GET" == method_string)
    {
        if ("/index.html" == url_string)
        {
            const auto page = create_index_page(url_string);
            auto response = MHD_create_response_from_buffer(page.length(),
                const_cast<char*>(page.c_str()),
                MHD_RESPMEM_MUST_COPY);
            std::unique_ptr<MHD_Response, decltype(MHD_destroy_response)*> up_response{response,
                MHD_destroy_response};
            return MHD_queue_response(connection, MHD_HTTP_OK, up_response.get());
        }
        else if ("/another.html" == url_string)
        {
            const auto page = create_another_page(url_string);
            auto response = MHD_create_response_from_buffer(page.length(),
                const_cast<char*>(page.c_str()),
                MHD_RESPMEM_MUST_COPY);
            std::unique_ptr<MHD_Response, decltype(MHD_destroy_response)*> up_response{response,
                MHD_destroy_response};
            return MHD_queue_response(connection, MHD_HTTP_OK, up_response.get());
        }
    }
    
    const auto page = create_not_found_page(url_string);
    auto response = MHD_create_response_from_buffer(page.length(),
        const_cast<char*>(page.c_str()),
        MHD_RESPMEM_MUST_COPY);
    std::unique_ptr<MHD_Response, decltype(MHD_destroy_response)*> up_response{response,
        MHD_destroy_response};
    return MHD_queue_response(connection, MHD_HTTP_NOT_FOUND, up_response.get());
}