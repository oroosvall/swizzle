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

    std::string toMemoryString(uint64_t value) noexcept
    {
        const char* units[] = {"Bytes", "KB", "MB"};
        int unitCount = sizeof(units) / sizeof(units[0]) - 1;
        int idx = 0;

        while (value >= 1024 && idx < unitCount)
        {
            value = value / 1024;
            idx++;
        }

        std::string str = std::to_string(value) + units[idx];

        return str;
    }

} // namespace utils
