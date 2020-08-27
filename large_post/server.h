#pragma once

// C++ Standard Library
#include <atomic>
#include <chrono>
#include <cstdio>
#include <cstring>
#include <experimental/string_view>
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <string_view>
#include <stdexcept>

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

namespace Jade
{

class Server
{
public:
    static const uint16_t port{8888};
    static const std::size_t post_buffer_size{512};
    static const std::size_t maximum_clients{2};

    void run();

private:
    // See MHD_AccessHandlerCallback
    static int access_handler_callback_static(void *cls,
        MHD_Connection *connection,
        const char *url,
        const char *method,
        const char *version,
        const char *upload_data,
        size_t *upload_data_size,
        void **con_cls);

    static void request_completed_static(void *cls,
        MHD_Connection *connection,
        void **con_cls,
        MHD_RequestTerminationCode toe);

    static std::unique_ptr<MHD_Response, decltype(MHD_destroy_response)*> create_response(
        std::experimental::string_view content);
    
    int access_handler_callback(MHD_Connection *connection,
        const char *url,
        const char *method,
        const char *version,
        const char *upload_data,
        size_t *upload_data_size,
        void **con_cls);

    void request_completed(MHD_Connection *connection,
        void **con_cls,
        MHD_RequestTerminationCode toe);

    std::atomic<std::size_t> m_number_of_uploading_clients{};
    
    std::unique_ptr<MHD_Daemon, decltype(MHD_stop_daemon)*> m_daemon{nullptr, MHD_stop_daemon};
};

} // Jade