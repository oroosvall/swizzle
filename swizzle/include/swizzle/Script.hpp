#ifndef SCRIPT_HPP
#define SCRIPT_HPP

#include "Api.hpp"

namespace script
{
	class SWIZZLE_API Script
	{
	public:
		Script();
		virtual ~Script();

		void load(const char* binary);
		void loadFromFile(const char* binaryFile);
		void compile(const char* source);
		void compileFromFile(const char* sourceFile);



	};

}

#endif