#ifndef SHADER_PIPELINE_HPP
#define SHADER_PIPELINE_HPP

/* Include files */

#include <swizzle/gfx/Shader.hpp>

#include "_fwDecl.hpp"

#include "../backend/Vk.hpp"

#include <map>
#include <string>

/* Defines */

/* Typedefs/enums */

namespace vk
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
} // namespace swizzle::gfx

/* Forward Declared Structs/Classes */

/* Struct Declaration */

namespace vk
{
    struct Property
    {
        union
        {
            VkBlendFactor mBlendFactor[2];
            VkCullModeFlagBits mCullMode;
        };
    };
} // namespace swizzle::gfx

/* Class Declaration */

namespace vk
{
    class ShaderPipeline : public swizzle::gfx::Shader
    {
    public:
        ShaderPipeline(common::Resource<Device> device, const PresentFBO& frameBuffer,
                       const swizzle::gfx::ShaderAttributeList& attribList);
        virtual ~ShaderPipeline();

        virtual SwBool load(const SwChar* file) override;
        virtual common::Resource<swizzle::gfx::Material> createMaterial() override;

        VkPipeline getPipeline() const;
        VkPipelineLayout getPipelineLayout() const;

        VkDescriptorSet getDescriptorSet() const;

    private:

        void loadShader(const SwChar* shaderType, const SwChar* binaryFile);
        void createPipeline();

        void readProperty(const std::string& line);

        common::Resource<Device> mDevice;
        const PresentFBO& mFrameBuffer;
        const swizzle::gfx::ShaderAttributeList mShaderAttributes;

        VkPipelineLayout mPipelineLayout;
        VkPipeline mPipeline;

        std::map<ShaderModuleType, VkShaderModule> mShaders;
        std::map<PropertyType, Property> mProperties;

        VkDescriptorSetLayout mDescriptorLayout;
        VkDescriptorSet mDescriptorSet;

    };
}

/* Function Declaration */

#endif
