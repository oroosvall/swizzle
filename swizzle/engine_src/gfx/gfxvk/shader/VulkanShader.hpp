#ifndef VULKAN_SHADER_HPP
#define VULKAN_SHADER_HPP

/* Include files */

#include <swizzle/gfx/Shader.hpp>

#include "gfx/gfxvk/backend/VulkanObjectContainer.hpp"
#include "gfx/gfxvk/VulkanFrameBuffer.hpp"

#include <map>

/* Defines */

/* Typedefs/Enum */

namespace swizzle::gfx
{
    enum class ShaderModuleType
    {
        ShaderModuleType_Vertex,
        ShaderModuleType_Fragment
    };
}

/* Function Declaration*/

/* Forward Declared Structs/Classes */

/* Struct Declaration */

/* Class Declaration */

namespace swizzle::gfx
{
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