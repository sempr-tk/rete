#include "Util.hpp"

#include <sstream>
#include <algorithm>

namespace rete {
namespace util {


std::string ptrToStr(void const* ptr)
{
    std::stringstream ss;
    ss << ptr;
    return ss.str();
}

std::string dotEscape(const std::string& str)
{
    std::string cpy = str;
    for (size_t i = 0; (i = cpy.find("\"", i)) != std::string::npos;)
    {
        cpy.replace(i, 1, "\\\"");
        i += 2;
    }

    for (size_t i = 0; (i = cpy.find("<", i)) != std::string::npos;)
    {
        cpy.replace(i, 1, "\\<");
        i += 2;
    }

    for (size_t i = 0; (i = cpy.find(">", i)) != std::string::npos;)
    {
        cpy.replace(i, 1, "\\>");
        i += 2;
    }

    return cpy;
}


template <>
std::string to_string(const std::string& v)
{
    return v;
}

} /* util */
} /* rete */
