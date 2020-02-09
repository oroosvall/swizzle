#ifndef SHADER_HPP
#define SHADER_HPP

#include <cstdint>

namespace swizzle
{

	enum class eShaderProgramType
	{
		GraphicsProgram,
		ComputeProgram,
		RaytracingProgram
	};

	enum class ShaderType
	{
		VertexShader,
		GeometryShader,
		FragmentShader,
		TesselationControlShader,
		TesselationEvalShader,
		ComputeShader,
		
	};

	class ShaderProgram
	{
	public:
		virtual ~ShaderProgram() {}

		virtual eShaderProgramType getProgramType() const = 0;
		virtual bool isShaderTypeSupported(ShaderType type) const = 0;

		virtual bool load(const char* file) = 0;

	};

}

#endif