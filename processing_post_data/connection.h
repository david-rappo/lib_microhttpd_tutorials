#pragma once

// C++ Standard Library
#include <memory>
#include <string>

// libmicrohttpd
#include <microhttpd.h>

namespace Jade
{

class Connection
{
public:
    static const std::string method_post;
    static const std::string method_get;

    enum class connection_type
    {
        get,
        post
    };

    // See MHD_PostDataIterator
    static int post_data_iterator_static(void *cls,
        enum MHD_ValueKind kind,
        const char *key,
        const char *file_name,
        const char *content_type,
        const char *transfer_encoding,
        const char *data,
        uint64_t off,
        size_t size);
    
    int post_data_iterator(enum MHD_ValueKind kind,
        const char *key,
        const char *file_name,
        const char *content_type,
        const char *transfer_encoding,
        const char *data,
        uint64_t off,
        size_t size);

    connection_type get_connection_type() const;
    void set_connection_type(connection_type c);
    
    const std::string &get_answer_string() const;
    
    const MHD_PostProcessor *get_post_processor() const;
    MHD_PostProcessor *get_post_processor();
    void set_post_processor(std::unique_ptr<MHD_PostProcessor, decltype(MHD_destroy_post_processor)*> &&post_processor);

private:
    void set_answer_string(const std::string &answer_string);

    connection_type m_connection_type{};
    // MHD_PostProcessor is used to parse the data portion of a POST request.
    std::unique_ptr<MHD_PostProcessor, decltype(MHD_destroy_post_processor)*> m_post_processor{
        nullptr,
        MHD_destroy_post_processor};
    std::string m_answer_string;
};

} // Jade