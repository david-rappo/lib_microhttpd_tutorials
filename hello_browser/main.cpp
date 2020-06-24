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

// System
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/types.h>

// libmicrohttpd
#include <microhttpd.h>

// Guidelines Support Library
#include <gsl/gsl>

#include "create_message.h"

int answer_to_connection(void *cls,
    struct MHD_Connection *connection,
    const char *url,
    const char *method,
    const char *version,
    const char *upload_data,
    size_t *upload_data_size,
    void **con_cls);

int main(int argc, char **argv)
{
    if (2 != argc)
    {
        std::cout << "Parameter(s): <message>\n";
        return 0;
    }
    
    const char *message = *(argv + 1);
    const auto create_message = Jade::Create_message(message);
    constexpr uint16_t port{8888};
    // Flag MHD_USE_PEDANTIC_CHECKS is deprecated. It was replaced with MHD_OPTION_STRICT_FOR_CLIENT.
    // However, MHD_OPTION_STRICT_FOR_CLIENT does not seem to work.
    const auto flags = MHD_USE_AUTO|MHD_USE_INTERNAL_POLLING_THREAD|MHD_USE_PEDANTIC_CHECKS;
    auto daemon = MHD_start_daemon(flags,
        port,
        nullptr,
        nullptr,
        &answer_to_connection,
        const_cast<Jade::Create_message*>(&create_message),
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

int answer_to_connection(void *cls,
    struct MHD_Connection *connection,
    const char *url,
    const char *method,
    const char *version,
    const char *upload_data,
    size_t *upload_data_size,
    void **con_cls)
{
    const auto create_message = *reinterpret_cast<const Jade::Create_message*>(cls);
    const auto page = create_message();
    auto response = MHD_create_response_from_buffer(page.length(),
        const_cast<char*>(page.c_str()),
        MHD_RESPMEM_MUST_COPY);
    std::unique_ptr<MHD_Response, decltype(MHD_destroy_response)*> up_response{response,
        MHD_destroy_response};
    return MHD_queue_response(connection, MHD_HTTP_OK, up_response.get());
}