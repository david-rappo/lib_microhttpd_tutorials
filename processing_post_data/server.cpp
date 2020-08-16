// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "server.h"

// C++ Standard Library
#include <cassert>
#include <iostream>

// processing_post_data program
#include "connection.h"
#include "page.h"

namespace Jade
{

void Server::run()
{
    const auto flags{MHD_USE_AUTO|MHD_USE_INTERNAL_POLLING_THREAD};
    auto daemon = MHD_start_daemon(flags,
        port,
        nullptr,
        nullptr,
        Server::access_handler_callback_static,
        this,
        MHD_OPTION_NOTIFY_COMPLETED, // variadic list starts here
        Server::request_completed_static,
        this,
        MHD_OPTION_END);
    if (nullptr == daemon)
    {
        return;
    }

    std::unique_ptr<MHD_Daemon, decltype(MHD_stop_daemon)*> up_daemon{daemon, MHD_stop_daemon};
    m_daemon = std::move(up_daemon);
}

int Server::access_handler_callback_static(void *cls,
    MHD_Connection *connection,
    const char *url,
    const char *method,
    const char *version,
    const char *upload_data,
    size_t *upload_data_size,
    void **con_cls)
{
    assert(nullptr != cls);
    auto server = reinterpret_cast<Server*>(cls);
    return server->access_handler_callback(connection,
        url,
        method,
        version,
        upload_data,
        upload_data_size,
        con_cls);
}

void Server::request_completed_static(void *cls,
    MHD_Connection *connection,
    void **con_cls,
    MHD_RequestTerminationCode toe)
{
    assert(nullptr != cls);
    auto server = reinterpret_cast<Server*>(cls);
    server->request_completed(connection, con_cls, toe);
}

std::unique_ptr<MHD_Response, decltype(MHD_destroy_response)*> Server::create_response(std::string_view content)
{
    auto response = MHD_create_response_from_buffer(content.length(),
        const_cast<char*>(content.data()),
        MHD_RESPMEM_MUST_COPY);
    if (nullptr == response)
    {
        throw std::runtime_error{"Failed to create MHD_Response"};
    }
    
    std::unique_ptr<MHD_Response, decltype(MHD_destroy_response)*> up_response{response,
        MHD_destroy_response};
    return up_response;
}

// Returns MHD_NO if the connection was not handled. Otherwise returns MHD_YES.
int Server::access_handler_callback(MHD_Connection *connection,
    const char *url,
    const char *method,
    const char *version,
    const char *upload_data,
    size_t *upload_data_size,
    void **con_cls)
{
    assert(nullptr != con_cls);
    auto &extra_connection_data = *con_cls;
    if (nullptr == extra_connection_data)
    {
        const auto method_string = std::string{method};
        auto up_connection = std::make_unique<Jade::Connection>();
        if (Connection::method_post == method_string)
        {
            up_connection->set_connection_type(Connection::connection_type::post);
            auto post_processor = MHD_create_post_processor(connection,
                post_buffer_size,
                Connection::post_data_iterator_static,
                up_connection.get());
            if (nullptr == post_processor)
            {
                return MHD_NO;
            }
            
            std::unique_ptr<MHD_PostProcessor, decltype(MHD_destroy_post_processor)*> up_post_processor(
                post_processor,
                MHD_destroy_post_processor);
            up_connection->set_post_processor(std::move(up_post_processor));
        }
        else
        {
            up_connection->set_connection_type(Connection::connection_type::get);
        }

        // Notify up_connection to relinquish ownership of the instance of Jade::Connection. Server::request_completed
        // will handle deleting the instance of Jade::Connection.
        extra_connection_data = up_connection.release();
        return MHD_YES;
    }

    const auto method_string = std::string{method};
    if (Connection::method_get == method_string)
    {
        auto response = Server::create_response(Jade::Page::ask_page_content);
        return MHD_queue_response(connection, MHD_HTTP_OK, response.get());
    }

    if (Connection::method_post == method_string)
    {
        auto &extra_connection_data = *con_cls;
        auto extra_connection = reinterpret_cast<Connection*>(extra_connection_data);
        if (nullptr != upload_data_size)
        {
            const auto data_size = *upload_data_size;
            if (0 != data_size)
            {
                // New POST data is available. MHD_post_process will call Jade::Connection::post_data_iterator
                MHD_post_process(extra_connection->get_post_processor(),
                    upload_data,
                    data_size);
                *upload_data_size = 0;
                return MHD_YES;
            }
            else
            {
                // Assume that all of the POST data has been received. Send the response to the client.
                if (!extra_connection->get_answer_string().empty())
                {
                    auto response = Server::create_response(extra_connection->get_answer_string());
                    return MHD_queue_response(connection, MHD_HTTP_OK, response.get());
                }
            }
        }
    }
    
    auto response = Server::create_response(Jade::Page::error_page_content);
    return MHD_queue_response(connection, MHD_HTTP_OK, response.get());
}

void Server::request_completed(MHD_Connection *connection,
    void **con_cls,
    MHD_RequestTerminationCode toe)
{
    assert(nullptr != con_cls);
    auto &extra_connection_data = *con_cls;
    assert(nullptr != extra_connection_data);
    auto extra_connection = reinterpret_cast<Connection*>(extra_connection_data);
    delete extra_connection;
    extra_connection_data = nullptr;
}

} // Jade