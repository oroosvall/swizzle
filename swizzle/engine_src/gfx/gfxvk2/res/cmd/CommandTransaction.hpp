#ifndef COMMAND_TRANSACTION_HPP
#define COMMAND_TRANSACTION_HPP

/* Include files */

#include "../../backend/Vk.hpp"
#include "../VkResource.hpp"
#include <swizzle/gfx/CommandBuffer.hpp>

/* Defines */

/* Typedefs/enums */

/* Forward Declared Structs/Classes */

/* Struct Declaration */

/* Class Declaration */

namespace vk
{
    class VCommandTransaction : public swizzle::gfx::CommandTransaction
    {
    public:
        VCommandTransaction(VkCommandBuffer commandBuffer, const common::Resource<LifetimeToken> token);
        virtual ~VCommandTransaction();

        virtual void copyBuffer(common::Resource<swizzle::gfx::Buffer> to,
                                common::Resource<swizzle::gfx::Buffer> from) override;

        virtual void bindComputeShader(common::Resource<swizzle::gfx::Shader> shader) override;
        virtual void dispatchCompute(U32 groupX, U32 groupY, U32 groupZ) override;

        virtual void uploadTexture(common::Resource<swizzle::gfx::Texture> texture) override;

        VkCommandBuffer getBuffer() const;
        const common::Resource<LifetimeToken> getToken() const;
    private:
        VkCommandBuffer mCommandBuffer;
        const common::Resource<LifetimeToken> mLifetimeToken;
    };
} // namespace vk

/* Function Declaration */

#endif