#include <utils/StringUtils.hpp>

#include <string>
#include <cstring>

namespace utils
{
    size_t strlenSafe(const char* str) noexcept
    {
        size_t s = 0U;
        if (nullptr != str)
        {
            s = strlen(str);
        }
        return s;
    }

    size_t wstrlenSafe(const wchar_t* str) noexcept
    {
        size_t s = 0U;
        if (nullptr != str)
        {
            s = wcslen(str);
        }
        return s;
    }

} // namespace utils
