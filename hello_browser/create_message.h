#pragma once

// C++ Standard Library
#include <string>

namespace Jade
{

class Create_message
{
public:
    explicit Create_message(const std::string &str);
    std::string operator()() const;

private:
    const std::string &get_string() const;
    std::string m_string;
};

}