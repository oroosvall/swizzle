#ifndef COMMAND_BUFFER_HPP
#define COMMAND_BUFFER_HPP

/* Include files */

#include <common/Common.hpp>

#include <swizzle/gfx/Buffer.hpp>
#include <swizzle/gfx/FrameBuffer.hpp>
#include <swizzle/gfx/Material.hpp>
#include <swizzle/gfx/Swapchain.hpp>
#include <swizzle/gfx/Texture.hpp>

/* Defines */

/* Typedefs/enums */

/* Forward Declared Structs/Classes */

/* Struct Declaration */

namespace swizzle::gfx
{
    struct ScissorInfo
    {
        U32 enableCtr;
        S32 x, y;
        U32 w, h;
    };
} // namespace swizzle::gfx

/* Class Declaration */

namespace swizzle::gfx
{
    class DrawCommandTransaction
    {
    public:
        virtual ~DrawCommandTransaction() {}

        virtual void bindShader(common::Resource<Shader> shader) = 0;
        virtual void bindMaterial(common::Resource<Shader> shader, common::Resource<Material> material) = 0;
        virtual void setShaderConstant(common::Resource<Shader> shader, U8* data, U32 size) = 0;
        virtual void setViewport(U32 x, U32 y) = 0;
        virtual void enableStencilTest(SwBool enable) = 0;

        virtual void bindVertexBuffer(common::Resource<Buffer> buffer) = 0;
        virtual void bindIndexBuffer(common::Resource<Buffer> buffer, SwBool bitSize16) = 0;

        virtual void draw(common::Resource<Buffer> buffer) = 0;
        virtual void drawIndexed(common::Resource<Buffer> buffer, common::Resource<Buffer> index) = 0;

        virtual void drawInstanced(common::Resource<Buffer> buffer, common::Resource<Buffer> instanceData) = 0;
        virtual void drawIndexedInstanced(common::Resource<Buffer> buffer, common::Resource<Buffer> index,
                                          common::Resource<Buffer> instanceData) = 0;

        virtual void drawMeshlet(U32 meshletCount) = 0;

        virtual void drawNoBind(U32 vertexCount, U32 first) = 0;
        virtual void drawIndexedNoBind(U32 vertexCount, U32 first, U32 vertOffset) = 0;

        virtual const ScissorInfo pushScissorRegion(S32 x, S32 y, U32 w, U32 h) = 0;
        virtual void popScisorInfo(const ScissorInfo& sci) = 0;

        virtual void setScissor(S32 x, S32 y, U32 w, U32 h) = 0;
    };

    class CommandTransaction
    {
    public:
        virtual ~CommandTransaction() {}

        /// <summary>
        /// Performs a copy transfer operation from one buffer to another
        /// </summary>
        /// <param name="to">Destination buffer to copy data to</param>
        /// <param name="from">Source buffer to copy data from</param>
        virtual void copyBuffer(common::Resource<Buffer> to, common::Resource<Buffer> from) = 0;

        /// <summary>
        /// Bind a shader for compute operations
        /// </summary>
        /// <param name="shader">Compute shader to bind</param>
        virtual void bindComputeShader(common::Resource<Shader> shader, common::Resource<swizzle::gfx::Material> mat,
                                       U8* constants, U32 constantsSize) = 0;
        /// <summary>
        /// Execute compute operations on previous bound compute shader
        /// </summary>
        /// <param name="groupX"></param>
        /// <param name="groupY"></param>
        /// <param name="groupZ"></param>
        virtual void dispatchCompute(U32 groupX, U32 groupY, U32 groupZ) = 0;

        /// <summary>
        /// Perform transfer of texture data from CPU visible to device memory
        /// </summary>
        /// <param name="texture">Texture to upload to GPU</param>
        virtual void uploadTexture(common::Resource<Texture> texture) = 0;

        virtual void changeImageLayoutCompute(common::Resource<swizzle::gfx::Texture> texture) = 0;
        virtual void changeImageLayoutRender(common::Resource<swizzle::gfx::Texture> texture) = 0;
    };

    class CommandBuffer : public std::enable_shared_from_this<CommandBuffer>
    {
    public:
        virtual ~CommandBuffer() {}

        virtual U32 getDrawCount() = 0;
        virtual U64 getVertCount() = 0;
        virtual U64 getTriCount() = 0;

        virtual void reset(bool hardReset) = 0;

        /// <summary>
        /// Begin recording of transfer or compute commands
        /// </summary>
        /// <returns>Handle to command transactions</returns>
        virtual common::Unique<CommandTransaction> begin() = 0;
        /// <summary>
        /// End recording of transfer or compute commands
        /// </summary>
        /// <param name="transaction">Transaction handle obtained from previous call to begin</param>
        virtual void end(common::Unique<CommandTransaction>&& transaction) = 0;

        /// <summary>
        /// Begin a draw command transaction to record drawing commands
        /// Only valid api calls to drawing commands are possible
        /// </summary>
        /// <param name="swp">The swapchain to target for the draw commands</param>
        /// <param name="transaction">Transaction handle obtained from previous call to begin</param>
        /// <returns>Handle to draw command transactions</returns>
        virtual common::Unique<DrawCommandTransaction>
            beginRenderPass(common::Resource<Swapchain> swp, common::Unique<CommandTransaction>&& transaction) = 0;
        /// <summary>
        /// Begin a draw command transaction to record drawing commands
        /// Only valid api calls to drawing commands are possible
        /// </summary>
        /// <param name="swp">The framebuffer to target for the draw commands</param>
        /// <param name="transaction">Transaction handle obtained from previous call to begin</param>
        /// <returns>Handle to draw command transactions</returns>
        virtual common::Unique<DrawCommandTransaction>
            beginRenderPass(common::Resource<FrameBuffer> swp, common::Unique<CommandTransaction>&& transaction) = 0;
        /// <summary>
        /// End recording of drawing commands
        /// </summary>
        /// <param name="transaction">Handle to draw command transactions obtained from beginRenderPass</param>
        /// <returns>Handle to command transactions</returns>
        virtual common::Unique<CommandTransaction>
            endRenderPass(common::Unique<DrawCommandTransaction>&& transaction) = 0;
    };
} // namespace swizzle::gfx

/* Function Declaration */

#endif