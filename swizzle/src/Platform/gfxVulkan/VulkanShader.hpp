#ifndef VULKAN_SHADER_HPP
#define VULKAN_SHADER_HPP

#include "vk.hpp"
#include <swizzle/gfx/Shader.hpp>
#include "VulkanFrameBuffer.hpp"

#include <vector>
#include <map>

namespace swizzle
{

	class VulkanShaderProgram : public ShaderProgram
	{
		friend class VulkanCommandBuffer;
	public:
		VulkanShaderProgram(VkDevice device, BaseFrameBuffer* baseFbo, VkPipelineCache pipelineCache, eShaderProgramType pipelineType);
		virtual ~VulkanShaderProgram();

		virtual eShaderProgramType getProgramType() const override;
		virtual bool isShaderTypeSupported(ShaderType type) const override;

		virtual bool load(const char* file) override;

		virtual void compileShader(const char* sourceFile);
		virtual void compileShader(const char* source, uint32_t size);

		virtual bool isBinarySupported() const;

		virtual void loadShader(const char* shaderType, const char* binaryFile);
		virtual void loadShader(const char* binary, uint32_t size);

		virtual void buildShaderProgram();

	private:

		enum class shaderTypes
		{
			vertex,
			fragment
		};

		void createPipeline();

		VkDevice mDevice;
		BaseFrameBuffer* mBaseFbo;
		VkPipelineCache mCache;
		eShaderProgramType mPipelineType;

		VkPipelineLayout mPipelineLayout;
		VkPipeline mPipeline;

		std::map<shaderTypes, VkShaderModule> mShaders;

		//std::vector<>
	};

}

#endif