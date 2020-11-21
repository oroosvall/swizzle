#ifndef SCRIPT_HELPER_FUNCS_HPP
#define SCRIPT_HELPER_FUNCS_HPP

#include "ScriptTypes.hpp"

namespace script
{

    bool isSignedNumber(const langString& string);
    bool isUnsignedNumber(const langString& string);
    bool isFloatNumber(const langString& string);
    bool isDoubleNumber(const langString& string);

    bool isNumber(const langString& string);
    bool isNumber2(const langString& string);
    bool isString(langString string);
} // namespace script

#endif
