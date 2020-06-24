// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "create_message.h"

// C++ Standard Library
#include <chrono>
#include <ctime>
#include <sstream>

namespace Jade
{

Create_message::Create_message(const std::string &str): m_string{str}
{
    // Empty.
}

std::string Create_message::operator()() const
{
    using std::chrono::system_clock;
    
    std::ostringstream oss;
    oss << "<html><body>";
    const auto date_time = system_clock::now();
    const auto timer = system_clock::to_time_t(date_time);
    oss << std::ctime(&timer);
    oss << ": " << get_string();
    oss << "</body></html>";
    return oss.str();
}

const std::string &Create_message::get_string() const
{
    return m_string;
}

}