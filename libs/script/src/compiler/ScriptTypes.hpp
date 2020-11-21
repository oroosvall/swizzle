#ifndef SCRIPT_TYPES_HPP
#define SCRIPT_TYPES_HPP

#include <cstdint>
#include <string>

namespace script
{

    typedef std::string langString;

    struct Token
    {
        langString mToken;
        uint32_t mLineNumber;
        uint32_t mColumn;
    };

} // namespace script

#endif