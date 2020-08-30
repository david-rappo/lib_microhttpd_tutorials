#pragma once

// C++ Standard Library
#include <cstdio>
#include <experimental/string_view>
#include <memory>
#include <mutex>
#include <string>

// libmicrohttpd
#include <microhttpd.h>

namespace Jade
{

class Connection
{
public:
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
    
    int get_http_status_code() const;
    void set_http_status_code(int http_status_code);

    connection_type get_connection_type() const;
    void set_connection_type(connection_type c);
    
    const std::string &get_answer_string() const;
    void set_answer_string(std::experimental::string_view answer_string);
    
    const MHD_PostProcessor *get_post_processor() const;
    MHD_PostProcessor *get_post_processor();
    void set_post_processor(std::unique_ptr<MHD_PostProcessor, decltype(MHD_destroy_post_processor)*> &&post_processor);

    const FILE *get_file() const;
    FILE *get_file();
    void set_file(std::unique_ptr<FILE, decltype(std::fclose)*> &&file);

    void close_file();

private:
    static std::mutex file_mutex;

    // Any value other than zero means that either:
    // 1) An error of some sort occurred
    // 2) The file upload completed
    int m_http_status_code{};
    connection_type m_connection_type{};
    // MHD_PostProcessor is used to parse the data portion of a POST request.
    std::unique_ptr<MHD_PostProcessor, decltype(MHD_destroy_post_processor)*> m_post_processor{
        nullptr,
        MHD_destroy_post_processor};
    std::unique_ptr<FILE, decltype(std::fclose)*> m_file{nullptr, std::fclose};
    std::string m_answer_string;
};

} // Jade