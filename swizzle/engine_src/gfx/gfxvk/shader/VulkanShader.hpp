#ifndef VULKAN_SHADER_HPP
#define VULKAN_SHADER_HPP

/* Include files */

#include <swizzle/gfx/Shader.hpp>

#include "gfx/gfxvk/backend/VkContainer.hpp"
#include "gfx/gfxvk/VulkanPresentFBO.hpp"

#include <map>
#include <string>

/* Defines */

/* Typedefs/Enum */

namespace swizzle::gfx
{
    enum class ShaderModuleType
    {
        ShaderModuleType_Vertex,
        ShaderModuleType_Fragment
    };

    enum class PropertyType
    {
        SourceBlend,
        DestinationBlend,
        CullMode,
    };
}

/* Function Declaration*/

/* Forward Declared Structs/Classes */

/* Struct Declaration */

namespace swizzle::gfx
{
    struct Property
    {
        union
        {
            VkBlendFactor mBlendFactor[2];

            VkCullModeFlagBits mCullMode;
        };

    };
}

/* Class Declaration */

namespace swizzle::gfx
{
    class VulkanShader : public Shader
    {
    public:

        VulkanShader(const VkContainer vkObjects, const PresentFBO& frameBuffer, const ShaderAttributeList& attribList);
        virtual ~VulkanShader();

        virtual SwBool load(const SwChar* filePath) override;

        virtual core::Resource<Material> createMaterial() override;

        VkPipeline getPipeline() const;
        VkPipelineLayout getPipelineLayout() const;

        VkDescriptorSet getDescriptorSet() const;

    private:

        void loadShader(const SwChar* shaderType, const SwChar* binaryFile);
        void createPipeline();

        void readProperty(const std::string& line);

        const VkContainer mVkObjects;
        const PresentFBO& mFrameBuffer;
        const ShaderAttributeList mShaderAttributes;

        VkPipelineLayout mPipelineLayout;
        VkPipeline mPipeline;

        std::map<ShaderModuleType, VkShaderModule> mShaders;
        std::map<PropertyType, Property> mProperties;

        VkDescriptorSetLayout mDescriptorLayout;
        VkDescriptorSet mDescriptorSet;

    };
}

#endif