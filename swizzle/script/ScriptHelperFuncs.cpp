#include "ScriptHelperFuncs.hpp"

namespace script {

    bool isSignedNumber(const langString_t& string) {
        const char* str = string.c_str();
        size_t strLen = string.size();

        for (size_t i = 0; i < strLen; i++) {

            if (!(str[i] - '0' >= 0 && str[i] - '0' <= 9)) {
                return false;
            }
        }
        return true;
    }

    bool isUnsignedNumber(const langString_t& string) {
        const char* str = string.c_str();
        if (string.size() >= 2) {
            size_t strLen = string.size() - 1;

            for (size_t i = 0; i < strLen; i++) {

                if (!(str[i] - '0' >= 0 && str[i] - '0' <= 9)) {
                    return false;
                }
            }

            // check to ensure it contains 'U' or 'u'
            if (str[strLen] == 'U' || str[strLen] == 'u') {
                return true;
            }
        }
        return false;
    }

    bool isFloatNumber(const langString_t& string) {
        const char* str = string.c_str();
        if (string.size() >= 2) {
            size_t strLen = string.size() - 1;

            for (size_t i = 0; i < strLen; i++) {

                if (!(str[i] - '0' >= 0 && str[i] - '0' <= 9)) {
                    if (!(strLen > 1 && str[i] == '.')) {
                        return false;
                    }
                }
            }

            // check to ensure it contains 'U' or 'u'
            if (str[strLen] == 'F' || str[strLen] == 'f') {
                return true;
            }
        }
        return false;
    }

    bool isDoubleNumber(const langString_t& string) {
        const char* str = string.c_str();
        size_t strLen = string.size();

		bool found = false;
        for (size_t i = 0; i < strLen; i++) {

            if (!(str[i] - '0' >= 0 && str[i] - '0' <= 9)) {
                if (!(strLen > 1 && str[i] == '.')) {
					return false;
				} else {
					found = true;
				}
            }
        }
        return found && true;
    }

    bool isNumber(const langString_t& string) {
        const char* str = string.c_str();
        size_t strLen = string.size();

        for (size_t i = 0; i < strLen; i++) {

            if (!(str[i] - '0' >= 0 && str[i] - '0' <= 9)) {
                if (!(strLen > 1 && str[i] == '.')) {
                    return false;
                }
            }
        }
        return true;
    }

    bool isNumber2(const langString_t& string) {
        return isSignedNumber(string) | isUnsignedNumber(string) | isFloatNumber(string) | isDoubleNumber(string);
    }

    bool isString(langString_t string) {
        return (string.size() > 2 && (string[0] == '\"' && string[string.size() - 1] == '\"'));
    }

} // namespace script