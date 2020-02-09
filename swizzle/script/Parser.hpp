#ifndef PARSER_HPP
#define PARSER_HPP

#include "ScriptTypes.hpp"
#include <vector>

namespace script
{


	std::vector<Token> Parse(langString_t code);

}

#endif