// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "connection.h"

// C++ Standard Library
#include <cassert>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <utility>

// processing_post_data program
#include "page.h"

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
    if (Jade::Page::name_field == key_string)
    {
        if ((size > 0) && (size <= Jade::Page::maximum_name_size))
        {
            assert(nullptr != data);
            const auto greeting_page_content = Jade::Page::create_greeting_page_content(data);
            set_answer_string(greeting_page_content);
        }
        else
        {
            set_answer_string(std::string{});
        }

        // Stop iterating.
        return MHD_NO;
    }
    
    // Continue iterating.
    return MHD_YES;
}

int Connection::get_http_status_code() const
{
    return m_http_status_code;
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

void Connection::set_answer_string(const std::string &answer_string)
{
    m_answer_string = answer_string;
}

} // Jade