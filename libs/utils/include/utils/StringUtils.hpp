
#ifndef STRING_UTILS_HPP
#define STRING_UTILS_HPP

#include <cstdint>
#include <cstddef>

namespace utils
{

    size_t strlenSafe(const char* str) noexcept;
    size_t wstrlenSafe(const wchar_t* str) noexcept;
}

#endif