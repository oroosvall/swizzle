#include <utils/StringUtils.hpp>

#include <string>

namespace utils
{
    size_t strlen_safe(const char* str)
    {
        size_t s = 0U;
        if (nullptr != str)
        {
            s = strlen(str);
        }
        return s;
    }
} // namespace utils
