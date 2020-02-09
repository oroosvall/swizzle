#include "VulkanRenderer.hpp"

#include "VulkanBuffer.hpp"
#include "VulkanCommandBuffer.hpp"
#include "VulkanFrameBuffer.hpp"
#include "VulkanShader.hpp"

#include "Debugging/Profile.hpp"

#include <fstream> // replace with something better

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace swizzle
{
	const char* cacheFileName = "cache.dat";

	VulkanRenderer::VulkanRenderer(Window* window)
		: mInstance(nullptr)
		, mPhysical(nullptr)
		, mLogical(nullptr)
		, mSwapchain(nullptr)
		, mHadSubmission(false)
	{
		mInstance = new VulkanInstance(window);

		auto devices = mInstance->listDevices();
		if (devices.size() == 0)
		{
			delete mInstance;
			// better error handling
			throw "No devices found";
		}

		window->addEventListener(this);

		mPhysical = new PhysicalDevice(devices[0]);
		// mPhysical->listAvailableExtensions(); // use this to check if an extension is supported
		mPhysical->activateExtension(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
		//uint32_t numQueues = mPhysical->getNumQueues();
		mPhysical->getNumQueues();
		std::vector<float> prios;
		prios.push_back(1.0F);
		mLogical = mPhysical->createLogicalDevice(prios);
		mMemoryHelper = new VulkanMemoryHelper(mLogical->getPhysical());

		mSwapchain = mLogical->createSwapchain(mInstance->getSurface(), *mMemoryHelper);


		size_t size = 0;
		char* data = nullptr;

		std::ifstream cacheFile;
		cacheFile.open(cacheFileName, std::ios::binary | std::ios::ate);
		if (cacheFile.is_open())
		{
			size = cacheFile.tellg();
			cacheFile.seekg(0, std::ios::beg);
			data = new char[size];
			cacheFile.read(data, size);
			cacheFile.close();
		}

		VkPipelineCacheCreateInfo cacheInfo;
		cacheInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
		cacheInfo.pNext = VK_NULL_HANDLE;
		cacheInfo.flags = 0;
		cacheInfo.initialDataSize = size;
		cacheInfo.pInitialData = data;

		if (data)
		{
			delete[] data;
			data = nullptr;
		}

		vkCreatePipelineCache(mLogical->getLogical(), &cacheInfo, nullptr, &mPipelineCache);
		
	}

	VulkanRenderer::~VulkanRenderer()
	{
		size_t size = 0U;
		vkGetPipelineCacheData(mLogical->getLogical(), mPipelineCache, &size, VK_NULL_HANDLE);
		char* cacheData = new char[size];
		vkGetPipelineCacheData(mLogical->getLogical(), mPipelineCache, &size, cacheData);

		std::ofstream cacheFile;
		cacheFile.open(cacheFileName, std::ios::binary);
		cacheFile.write(cacheData, size);
		cacheFile.close();

		delete [] cacheData;

		vkDestroyPipelineCache(mLogical->getLogical(), mPipelineCache, nullptr);

		delete mSwapchain;
		delete mLogical;
		delete mPhysical;
		delete mInstance;
		
		mSwapchain = nullptr;
		mLogical = nullptr;
		mPhysical = nullptr;
		mInstance = nullptr;
	}
	
	Resource<Buffer> VulkanRenderer::createBuffer(eBufferType bufferType)
	{
		return std::make_shared<VulkanBuffer>(mLogical->getLogical(), *mMemoryHelper, bufferType);
	}

	Resource<CommandBuffer> VulkanRenderer::createCommandBuffer(eCmdBufferType cmdBufType)
	{
		return std::make_shared<VulkanCommandBuffer>(mLogical->getLogical(), mLogical->getCommandPool(), cmdBufType);
	}

	Resource<FrameBuffer> VulkanRenderer::createFrameBuffer(uint32_t numAttach, DepthType depth, uint32_t width, uint32_t height)
	{
		return std::make_shared<VulkanFrameBuffer>(mLogical->getLogical(), numAttach,  depth, width, height, std::vector<Resource<VulkanTexture>>(), *mMemoryHelper);
	}

	Resource<FrameBuffer> VulkanRenderer::getDefaultFramebuffer()
	{
		return mSwapchain->getCurrentFramebuffer();
	}

	Resource<Texture2D> VulkanRenderer::createTexture(eTextureDataFormat type)
	{
		type;
		return nullptr;
	}
	
	Resource<ShaderProgram> VulkanRenderer::createShaderProgram(const Resource<FrameBuffer>& fbo, eShaderProgramType programType)
	{
		FrameBuffer* fb = fbo.get();
		BaseFrameBuffer* bfb = (BaseFrameBuffer*)fb;
		return std::make_shared<VulkanShaderProgram>(mLogical->getLogical(), bfb, mPipelineCache, programType);
	}

	void VulkanRenderer::submit(Resource<CommandBuffer> cmdbuffer)
	{
		CommandBuffer* cmdBuff = cmdbuffer.get();

		VulkanCommandBuffer* cmd = static_cast<VulkanCommandBuffer*>(cmdBuff);

		auto b = cmd->getCommandBuffer();

		VkSubmitInfo subinfo;
		subinfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		subinfo.pNext = 0;
		subinfo.waitSemaphoreCount = 0;
		subinfo.pWaitSemaphores = VK_NULL_HANDLE;
		subinfo.pWaitDstStageMask = VK_NULL_HANDLE;
		subinfo.commandBufferCount = 1;
		subinfo.pCommandBuffers = &b;
		subinfo.signalSemaphoreCount = 0;
		subinfo.pSignalSemaphores = VK_NULL_HANDLE;

		vkQueueSubmit(mLogical->getQueue(0, 0), 1, &subinfo, VK_NULL_HANDLE);

	}

	void VulkanRenderer::prepare(Resource<CommandBuffer> cmdbuf)
	{
		mSwapchain->prepare();

		auto fbo = mSwapchain->getCurrentFramebuffer();

		auto cmd = reinterpret_cast<VulkanCommandBuffer*>(cmdbuf.get());
		auto buffer = cmd->getCommandBuffer();
		
		VkImageMemoryBarrier imgBarrier;
		imgBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		imgBarrier.pNext = VK_NULL_HANDLE;
		imgBarrier.srcAccessMask = VkAccessFlagBits::VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		imgBarrier.dstAccessMask = VkAccessFlagBits::VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		imgBarrier.oldLayout = VkImageLayout::VK_IMAGE_LAYOUT_UNDEFINED;
		imgBarrier.newLayout = VkImageLayout::VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		imgBarrier.srcQueueFamilyIndex = 0;
		imgBarrier.dstQueueFamilyIndex = 0;
		imgBarrier.image = fbo->getImage(0U);

		imgBarrier.subresourceRange.aspectMask = VkImageAspectFlagBits::VK_IMAGE_ASPECT_COLOR_BIT;
		imgBarrier.subresourceRange.baseMipLevel = 0;
		imgBarrier.subresourceRange.levelCount = 1;
		imgBarrier.subresourceRange.baseArrayLayer = 0;
		imgBarrier.subresourceRange.layerCount = 1;

		vkCmdPipelineBarrier(buffer, VkPipelineStageFlagBits::VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT,
                                     VkPipelineStageFlagBits::VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT,
                                     VkDependencyFlagBits::VK_DEPENDENCY_BY_REGION_BIT, 0, VK_NULL_HANDLE, 0,
                                     VK_NULL_HANDLE, 1, &imgBarrier);

		mHadSubmission = false;
	}

	void VulkanRenderer::present()
	{
		if (!mHadSubmission)
		{
			
		}

		mSwapchain->present();
		
	}

	bool VulkanRenderer::isVsyncModeSupported(VSyncTypes sync) const
	{
		return mSwapchain->hasPresentMode(sync);
	}

	void VulkanRenderer::setVsync(VSyncTypes sync)
	{
		mSwapchain->setPresentMode(sync);
	}

	VkInstance VulkanRenderer::getInstance() const
	{
		return VK_NULL_HANDLE;
	}

	void VulkanRenderer::publishEvent(const swizzle::WindowEvent& evt)
	{
		if (evt.getEventType() == eWindowEventType::eType_resize)
		{
			mSwapchain->resize();
		}
	}

}