#ifndef SCRIPT_TYPES_HPP
#define SCRIPT_TYPES_HPP

#include <cstdint>
#include <string>

namespace script
{

	typedef std::string langString_t;

	struct Token
	{
		langString_t mToken;
		uint32_t mLineNumber;
		uint32_t mColumn;
	};

}

#endif