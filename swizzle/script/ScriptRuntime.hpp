#ifndef SCRIPT_RUNTIME_HPP
#define SCRIPT_RUNTIME_HPP

#include <cstdint>

namespace script
{
	struct ScriptContext;

	enum class Types
	{
		eTypeVoid,
		eTypeInt,
		eTypeChar,
		eTypeUnsignedInt,
		eTypeFloat,
		eTypeDouble,
		eTypeBool,
		eTypeUser
	};

	struct Value
	{
		Types mType;
		bool mAsReference;
		uint8_t mIsPointer;
		void* mValue;
	};

	struct Argument
	{
		uint8_t mNumArgs;
		Value* mArgs;
	};

	typedef Value(*function_t)(Argument);

	struct FunctionDescriptor
	{
		const char* mName;
		function_t mFunction;
		Value mExpectedReturnType;
		Argument mExpectedArgumentTypes;
	};

	ScriptContext* InitializeScriptRuntime();
	void CleanupScriptRuntime(ScriptContext* ctx);

	uint32_t RegisterFunction(FunctionDescriptor& function);
	uint32_t RegisterFunction(ScriptContext* ctx, FunctionDescriptor& function);

	void UnregisterFunction(uint32_t functionId);
	void UnregisterFunction(ScriptContext* ctx, uint32_t functionId);

}

#endif

