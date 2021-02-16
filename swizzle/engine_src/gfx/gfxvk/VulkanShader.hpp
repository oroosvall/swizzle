#ifndef VULKAN_SHADER_HPP
#define VULKAN_SHADER_HPP

#include <swizzle/gfx/Shader.hpp>
#include "backend/VulkanObjectContainer.hpp"
#include "VulkanFrameBuffer.hpp"

#include <map>

namespace swizzle::gfx
{

    enum class ShaderModuleType
    {
        ShaderModuleType_Vertex,
        ShaderModuleType_Fragment
    };

    class VulkanShader : public Shader
    {
    public:

        VulkanShader(const VulkanObjectContainer& vkObjects, const PresentFrameBuffer& frameBuffer, const ShaderAttributeList& attribList);
        virtual ~VulkanShader();

        virtual SwBool load(const SwChar* filePath) override;

        virtual core::Resource<Material> createMaterial() override;

        VkPipeline getPipeline() const;
        VkPipelineLayout getPipelineLayout() const;

        VkDescriptorSet getDescriptorSet() const;

    private:

        void loadShader(const SwChar* shaderType, const SwChar* binaryFile);
        void createPipeline();

        const VulkanObjectContainer& mVkObjects;
        const PresentFrameBuffer& mFrameBuffer;
        const ShaderAttributeList mShaderAttributes;

        VkPipelineLayout mPipelineLayout;
        VkPipeline mPipeline;

        std::map<ShaderModuleType, VkShaderModule> mShaders;

        VkDescriptorSetLayout mDescriptorLayout;
        VkDescriptorSet mDescriptorSet;

    };


}

#endif