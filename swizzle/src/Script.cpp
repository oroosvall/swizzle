#include <swizzle/Script.hpp>

#include <Script/Parser.hpp>

#include <fstream>
#include <sstream>

namespace script
{
	Script::Script() {}
	Script::~Script() {}

	void Script::load(const char* binary)
	{
		binary;
	}
	void Script::loadFromFile(const char* binaryFile)
	{
		binaryFile;
	}
	
	void Script::compile(const char* source)
	{
		auto tokens = script::Parse(source);
		tokens;
	}

	void Script::compileFromFile(const char* sourceFile)
	{
		std::ifstream t(sourceFile);
		std::stringstream buffer;
		buffer << t.rdbuf();
		compile(buffer.str().c_str());
	}


}