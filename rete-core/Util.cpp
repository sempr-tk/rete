#include "Util.hpp"

#include <sstream>
#include <algorithm>

#ifdef __GNUG__
#include <cxxabi.h>
#endif

namespace rete {
namespace util {

#if __GNUG__
    std::string demangle(const char* name)
    {
        int status;
        std::unique_ptr<char, void(*)(void*)> res {
            abi::__cxa_demangle(name, nullptr, nullptr, &status),
            std::free
        };

        return (status == 0 ? res.get() : name);
    }
#else
    std::string demangle(const char* name)
    {
        return name;
    }
#endif


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

    for (size_t i = 0; (i = cpy.find("{", i)) != std::string::npos;)
    {
        cpy.replace(i, 1, "\\{");
        i += 2;
    }

    for (size_t i = 0; (i = cpy.find("}", i)) != std::string::npos;)
    {
        cpy.replace(i, 1, "\\}");
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
