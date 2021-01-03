#include <swizzle/core/Logging.hpp>
#include <swizzle/core/Platform.hpp>

#include "VulkanRenderer.hpp"

#include <fstream> // replace with something better
#include <string>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace swizzle::gfx
{
    VulkanRenderer::VulkanRenderer()
        : mInstance(nullptr)
        , mPhysical(nullptr)
        , mLogical(nullptr)
        , mVkObjects()
        //, mLogical(nullptr)
        //, mSwapchain(nullptr)
        //, mHadSubmission(false)
    {
        mInstance = new VulkanInstance();

        auto devices = mInstance->listDevices();
        if (devices.size() == 0)
        {
            delete mInstance;
            // better error handling
            throw "No devices found";
        }

        //window->addEventListener(this);

        mPhysical = new PhysicalDevice(devices[0]);
        auto extensions = mPhysical->listAvailableExtensions(); // use this to check if an extension is supported
        for (auto it : extensions)
        {
            LOG_INFO("Found extension %s", it.c_str());
        }
        mPhysical->activateExtension(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
        // uint32_t numQueues = mPhysical->getNumQueues();
        mPhysical->getNumQueues();
        std::vector<float> prios;
        prios.push_back(1.0F);
        mLogical = mPhysical->createLogicalDevice(prios);
        //mMemoryHelper = new VulkanMemoryHelper(mLogical->getPhysical());

        //mSwapchain = mLogical->createSwapchain(mInstance->getSurface(), *mMemoryHelper);

        U64 size = 0;
        U8* data = nullptr;

        std::ifstream cacheFile;
        std::string cacheFilePath = core::GetAppCacheDirectory();
        cacheFilePath += "shaderCache.dat";

        cacheFile.open(cacheFilePath, std::ios::binary | std::ios::ate);
        if (cacheFile.is_open())
        {
            size = cacheFile.tellg();
            cacheFile.seekg(0, std::ios::beg);
            data = new U8[size];
            cacheFile.read((char*)data, size);
            cacheFile.close();
        }

        // Populate Vulkan object structure
        mVkObjects.mInstance = mInstance->getInstance();
        mVkObjects.mPhysicalDevice = mLogical->getPhysical();
        mVkObjects.mLogicalDevice = mLogical->getLogical();
        mVkObjects.mQueue = mLogical->getQueue(0, 0);
        mVkObjects.mQueueFamilyIndex = 0;

        vkGetPhysicalDeviceMemoryProperties(mVkObjects.mPhysicalDevice, &mVkObjects.mMemoryProperties);

        VkCommandPoolCreateInfo cmdPoolCreateInfo = {};
        cmdPoolCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        cmdPoolCreateInfo.pNext = VK_NULL_HANDLE;
        cmdPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        cmdPoolCreateInfo.queueFamilyIndex = mVkObjects.mQueueFamilyIndex;

        VkPipelineCacheCreateInfo pipelineCacheCreateInfo = {};
        pipelineCacheCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
        pipelineCacheCreateInfo.pNext = VK_NULL_HANDLE;
        pipelineCacheCreateInfo.flags = 0;
        pipelineCacheCreateInfo.initialDataSize = size;
        pipelineCacheCreateInfo.pInitialData = data;
        vkCreatePipelineCache(mVkObjects.mLogicalDevice, &pipelineCacheCreateInfo, nullptr, &mVkObjects.mPiplineCache);

        vkCreateCommandPool(mVkObjects.mLogicalDevice, &cmdPoolCreateInfo, nullptr, &mVkObjects.mCmdPool);

        mVkObjects.stageCmdBuffer = new StageCmdBuffer(mVkObjects.mLogicalDevice, mVkObjects.mCmdPool, mVkObjects.mQueue, mVkObjects.mMemoryProperties);

        if (data)
        {
            delete[] data;
            data = nullptr;
        }

        VkDescriptorPoolSize poolSize{};
        poolSize.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        poolSize.descriptorCount = 10U;

        VkDescriptorPoolCreateInfo descriptorPoolCreateInfo;
        descriptorPoolCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        descriptorPoolCreateInfo.pNext = VK_NULL_HANDLE;
        descriptorPoolCreateInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
        descriptorPoolCreateInfo.maxSets = 10U;
        descriptorPoolCreateInfo.poolSizeCount = 1U;
        descriptorPoolCreateInfo.pPoolSizes = &poolSize;

        vkCreateDescriptorPool(mVkObjects.mLogicalDevice, &descriptorPoolCreateInfo, nullptr, &mVkObjects.mDescriptorPool);

    }

    VulkanRenderer::~VulkanRenderer()
    {
        delete mVkObjects.stageCmdBuffer;
        mVkObjects.stageCmdBuffer = nullptr;

        vkDestroyDescriptorPool(mVkObjects.mLogicalDevice, mVkObjects.mDescriptorPool, nullptr);
        mVkObjects.mDescriptorPool = VK_NULL_HANDLE;

        vkDestroyCommandPool(mVkObjects.mLogicalDevice, mVkObjects.mCmdPool, nullptr);
        mVkObjects.mCmdPool = VK_NULL_HANDLE;

        size_t size = 0U;
        vkGetPipelineCacheData(mLogical->getLogical(), mVkObjects.mPiplineCache, &size, VK_NULL_HANDLE);
        U8* cacheData = new U8[size];
        vkGetPipelineCacheData(mLogical->getLogical(), mVkObjects.mPiplineCache, &size, cacheData);

        std::ofstream cacheFile;
        std::string cacheFilePath = core::GetAppCacheDirectory();
        cacheFilePath += "shaderCache.dat";
        cacheFile.open(cacheFilePath, std::ios::binary);
        cacheFile.write((char*)cacheData, size);
        cacheFile.close();

        delete[] cacheData;

        vkDestroyPipelineCache(mLogical->getLogical(), mVkObjects.mPiplineCache, nullptr);

        //delete mSwapchain;
        delete mLogical;
        delete mPhysical;
        delete mInstance;

        //mSwapchain = nullptr;
        mLogical = nullptr;
        mPhysical = nullptr;
        mInstance = nullptr;
    }

    const VulkanObjectContainer& VulkanRenderer::getVkObjects()
    {
        return mVkObjects;
    }

    //Resource<FrameBuffer> VulkanRenderer::createFrameBuffer(uint32_t numAttach, eDepthType depth, uint32_t width,
    //                                                        uint32_t height)
    //{
    //    return std::make_shared<VulkanFrameBuffer>(mLogical->getLogical(), numAttach, depth, width, height,
    //                                               std::vector<Resource<VulkanTexture>>(), *mMemoryHelper);
    //}

    //Resource<FrameBuffer> VulkanRenderer::getDefaultFramebuffer()
    //{
    //    return mSwapchain->getCurrentFramebuffer();
    //}

    //Resource<Texture2D> VulkanRenderer::createTexture(eTextureDataFormat type)
    //{
    //    type;
    //    return nullptr;
    //}

    //Resource<ShaderProgram> VulkanRenderer::createShaderProgram(const Resource<FrameBuffer>& fbo,
    //                                                            eShaderProgramType programType,
    //                                                            ShaderAttributeList& attributes)
    //{
    //    FrameBuffer* fb = fbo.get();
    //    BaseFrameBuffer* bfb = (BaseFrameBuffer*)fb;
    //    return std::make_shared<VulkanShaderProgram>(mLogical->getLogical(), bfb, mPipelineCache, programType,
    //                                                 attributes);
    //}

    //void VulkanRenderer::submit(Resource<CommandBuffer> cmdbuffer)
    //{
    //    CommandBuffer* cmdBuff = cmdbuffer.get();

    //    VulkanCommandBuffer* cmd = static_cast<VulkanCommandBuffer*>(cmdBuff);

    //    auto b = cmd->getCommandBuffer();

    //    VkSubmitInfo subinfo;
    //    subinfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    //    subinfo.pNext = 0;
    //    subinfo.waitSemaphoreCount = 0;
    //    subinfo.pWaitSemaphores = VK_NULL_HANDLE;
    //    subinfo.pWaitDstStageMask = VK_NULL_HANDLE;
    //    subinfo.commandBufferCount = 1;
    //    subinfo.pCommandBuffers = &b;
    //    subinfo.signalSemaphoreCount = 0;
    //    subinfo.pSignalSemaphores = VK_NULL_HANDLE;

    //    vkQueueSubmit(mLogical->getQueue(0, 0), 1, &subinfo, VK_NULL_HANDLE);
    //}

    //void VulkanRenderer::prepare(Resource<CommandBuffer> cmdbuf)
    //{
    //    mSwapchain->prepare();

    //    auto fbo = mSwapchain->getCurrentFramebuffer();

    //    auto cmd = reinterpret_cast<VulkanCommandBuffer*>(cmdbuf.get());
    //    auto buffer = cmd->getCommandBuffer();

    //    VkImageMemoryBarrier imgBarrier;
    //    imgBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    //    imgBarrier.pNext = VK_NULL_HANDLE;
    //    imgBarrier.srcAccessMask = VkAccessFlagBits::VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    //    imgBarrier.dstAccessMask = VkAccessFlagBits::VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    //    imgBarrier.oldLayout = VkImageLayout::VK_IMAGE_LAYOUT_UNDEFINED;
    //    imgBarrier.newLayout = VkImageLayout::VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    //    imgBarrier.srcQueueFamilyIndex = 0;
    //    imgBarrier.dstQueueFamilyIndex = 0;
    //    imgBarrier.image = fbo->getImage(0U);

    //    imgBarrier.subresourceRange.aspectMask = VkImageAspectFlagBits::VK_IMAGE_ASPECT_COLOR_BIT;
    //    imgBarrier.subresourceRange.baseMipLevel = 0;
    //    imgBarrier.subresourceRange.levelCount = 1;
    //    imgBarrier.subresourceRange.baseArrayLayer = 0;
    //    imgBarrier.subresourceRange.layerCount = 1;

    //    vkCmdPipelineBarrier(buffer, VkPipelineStageFlagBits::VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT,
    //                         VkPipelineStageFlagBits::VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT,
    //                         VkDependencyFlagBits::VK_DEPENDENCY_BY_REGION_BIT, 0, VK_NULL_HANDLE, 0, VK_NULL_HANDLE, 1,
    //                         &imgBarrier);

    //    mHadSubmission = false;
    //}

    //void VulkanRenderer::present()
    //{
    //    if (!mHadSubmission)
    //    {
    //    }
    //    mSwapchain->present();
    //}

} // namespace swizzle