#ifndef SCRIPT_HELPER_FUNCS_HPP
#define SCRIPT_HELPER_FUNCS_HPP

#include "ScriptTypes.hpp"

namespace script {

	bool isSignedNumber(const langString_t& string);
	bool isUnsignedNumber(const langString_t& string);
	bool isFloatNumber(const langString_t& string);
	bool isDoubleNumber(const langString_t& string);

    bool isNumber(const langString_t& string);
	bool isNumber2(const langString_t& string);
    bool isString(langString_t string);
} // namespace script

#endif
