
#ifndef STRING_UTILS_HPP
#define STRING_UTILS_HPP

#include <cstdint>
#include <cstddef>

#include <string>

namespace utils
{

    size_t strlenSafe(const char* str) noexcept;
    size_t wstrlenSafe(const wchar_t* str) noexcept;

    std::string toMemoryString(uint64_t value) noexcept;

}

#endif