// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "connection.h"

// C++ Standard Library
#include <cassert>
#include <cstdio>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <utility>

// processing_post_data program
#include "page.h"

namespace Jade
{

std::mutex Connection::file_mutex{};

} // Jade

namespace Jade
{

int Connection::post_data_iterator_static(void *cls,
    enum MHD_ValueKind kind,
    const char *key,
    const char *file_name,
    const char *content_type,
    const char *transfer_encoding,
    const char *data,
    uint64_t off,
    size_t size)
{
    assert(nullptr != cls);
    auto connection = reinterpret_cast<Connection*>(cls);
    assert(nullptr != connection);
    return connection->post_data_iterator(kind,
        key,
        file_name,
        content_type,
        transfer_encoding,
        data,
        off,
        size);
}

// MHD_YES: Continue iterating.
// MHD_NO: Stop iterating.
int Connection::post_data_iterator(enum MHD_ValueKind kind,
    const char *key,
    const char *file_name,
    const char *content_type,
    const char *transfer_encoding,
    const char *data,
    uint64_t off,
    size_t size)
{
    assert(nullptr != key);
    const auto key_string = std::string{key};
    if (Jade::Page::file_field != key_string)
    {
        set_answer_string(Jade::Page::error_page_content);
        set_http_status_code(MHD_HTTP_BAD_REQUEST);
        return MHD_YES;
    }

    if (nullptr == get_file())
    {
        // An error occurred.
        if (0 != get_http_status_code())
        {
            return MHD_YES;
        }

        // The reason for locking file_mutex is that the tutorial (7 - Improved processing of POST data) mentioned a
        // possible race condition between the two calls to std::fopen.
        std::lock_guard<std::mutex> lock_guard{file_mutex};
        // Check if another client specified a file with the same name.
        std::cout << "Connection::post_data_iterator";
        std::cout << " Opening " << file_name << "...\n";
        auto file_handle = std::fopen(file_name, "rb");
        std::unique_ptr<FILE, decltype(std::fclose)*> up_file_handle{file_handle, std::fclose};
        if (nullptr != up_file_handle)
        {
            const auto page_content = Jade::Page::create_file_exists_content(file_name);
            set_answer_string(page_content);
            set_http_status_code(MHD_HTTP_FORBIDDEN);
            return MHD_YES;
        }

        // Open the file called file_name in "Append" mode to write more data to it.
        file_handle = std::fopen(file_name, "ab");
        up_file_handle.reset(file_handle);
        set_file(std::move(up_file_handle));
        if (nullptr == get_file())
        {
            set_answer_string(Jade::Page::file_input_output_error_page_content);
            set_http_status_code(MHD_HTTP_INTERNAL_SERVER_ERROR);
            return MHD_YES;
        }
    }
    
    if (size > 0)
    {
        std::cout << "Connection::post_data_iterator";
        std::cout << " Attempting to write " << size << " bytes to " << file_name << "\n";
        const auto result = std::fwrite(data, sizeof(char), size, get_file());
        if (0 == result)
        {
            set_answer_string(Jade::Page::file_input_output_error_page_content);
            set_http_status_code(MHD_HTTP_INTERNAL_SERVER_ERROR);
            return MHD_YES;
        }
    }

    return MHD_YES;
}

int Connection::get_http_status_code() const
{
    return m_http_status_code;
}

void Connection::set_http_status_code(int http_status_code)
{
    m_http_status_code = http_status_code;
}

Connection::connection_type Connection::get_connection_type() const
{
    return m_connection_type;
}

void Connection::set_connection_type(connection_type c)
{
    m_connection_type = c;
}

const std::string &Connection::get_answer_string() const
{
    return m_answer_string;
}

void Connection::set_answer_string(std::experimental::string_view answer_string)
{
    m_answer_string = std::string(answer_string.data(), answer_string.size());
}

const MHD_PostProcessor *Connection::get_post_processor() const
{
    return m_post_processor.get();
}

MHD_PostProcessor *Connection::get_post_processor()
{
    return m_post_processor.get();
}

void Connection::set_post_processor(
    std::unique_ptr<MHD_PostProcessor, decltype(MHD_destroy_post_processor)*> &&post_processor)
{
    m_post_processor = std::move(post_processor);
}

const FILE *Connection::get_file() const
{
    return m_file.get();
}

FILE *Connection::get_file()
{
    return m_file.get();
}

void Connection::set_file(std::unique_ptr<FILE, decltype(std::fclose)*> &&file)
{
    m_file = std::move(file);
}

void Connection::close_file()
{
    m_file.reset();
}

} // Jade