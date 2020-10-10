
#include "VulkanTexture.hpp"
#include "VulkanStagingBuffer.hpp"

namespace swizzle
{

	VulkanTexture::VulkanTexture(const TextureInfo& textureInfo)
		: mOwnImage(!textureInfo.createFromExternal)
		, mDevice(textureInfo.device)
		, mMemoryHelper(textureInfo.memHelper)
		, mTextureType(textureInfo.resourceType)
		, mImage(VK_NULL_HANDLE)
		, mImageView(VK_NULL_HANDLE)
		, mWidth(textureInfo.width)
		, mHeight(textureInfo.height)
		, mFormat(textureInfo.format)
		, mTextureBuffer(VK_NULL_HANDLE)
		, mTextureMemory(VK_NULL_HANDLE)
	{
		if (mOwnImage)
		{
			createImage();
		}
		else
		{
			mImage = textureInfo.exernalImages;
		}
		if (textureInfo.externalView == VK_NULL_HANDLE)
		{
			createImageView();
		}
		else
		{
			mImageView = textureInfo.externalView;
		}

	}

	VulkanTexture::~VulkanTexture()
	{
		vkDestroyImageView(mDevice, mImageView, nullptr);
		if (mOwnImage)
		{
			vkDestroyImage(mDevice, mImage, nullptr);
		}

		if (mTextureBuffer != VK_NULL_HANDLE)
		{
			vkDestroyBuffer(mDevice, mTextureBuffer, nullptr);
		}

		if (mTextureMemory != VK_NULL_HANDLE)
		{
			vkFreeMemory(mDevice, mTextureMemory, nullptr);
		}
	}

	/*eRenderApi VulkanTexture::getResourceApiType() const
	{
		return eRenderApi::eRenderer_Vulkan;
	}*/

	const uint32_t VulkanTexture::getWidth() const
	{
		return mWidth;
	}

	const uint32_t VulkanTexture::getHeight() const
	{
		return mHeight;
	}

	uint32_t VulkanTexture::getLayer() const
	{
		return 1U;
	}

	void VulkanTexture::setTextureData(uint32_t width, uint32_t height, void* data)
	{
		data;
		if (mTextureBuffer != VK_NULL_HANDLE)
		{
			vkDestroyBuffer(mDevice, mTextureBuffer, nullptr);
		}
		if (mTextureMemory != VK_NULL_HANDLE)
		{
			vkFreeMemory(mDevice, mTextureMemory, nullptr);
		}

		VkBufferCreateInfo bufferInfo;
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.pNext = VK_NULL_HANDLE;
		bufferInfo.flags = 0;
		bufferInfo.size = static_cast<VkDeviceSize>(width)* static_cast<VkDeviceSize>(height);
		bufferInfo.usage = VkBufferUsageFlagBits::VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
		bufferInfo.sharingMode = VkSharingMode::VK_SHARING_MODE_EXCLUSIVE;
		bufferInfo.queueFamilyIndexCount = 0;
		bufferInfo.pQueueFamilyIndices = 0;

		vkCreateBuffer(mDevice, &bufferInfo, nullptr, &mTextureBuffer);

		VkMemoryRequirements req;
		vkGetBufferMemoryRequirements(mDevice, mTextureBuffer, &req);

		VkMemoryAllocateInfo alloc_info = {};
		alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		alloc_info.allocationSize = req.size;
		alloc_info.memoryTypeIndex =
			mMemoryHelper.findMemoryType(VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, req.memoryTypeBits);
		vkAllocateMemory(mDevice, &alloc_info, nullptr, &mTextureMemory);

		vkBindBufferMemory(mDevice, mTextureBuffer, mTextureMemory, 0);
	}

	void VulkanTexture::setTextureData(void* data, uint32_t width, uint32_t height, uint32_t layer)
	{
		layer;
		data;
		if (mTextureBuffer != VK_NULL_HANDLE)
		{
			vkDestroyBuffer(mDevice, mTextureBuffer, nullptr);
		}
		if (mTextureMemory != VK_NULL_HANDLE)
		{
			vkFreeMemory(mDevice, mTextureMemory, nullptr);
		}

		VkBufferCreateInfo bufferInfo;
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.pNext = VK_NULL_HANDLE;
		bufferInfo.flags = 0;
		bufferInfo.size = static_cast<VkDeviceSize>(width)* static_cast<VkDeviceSize>(height);
		bufferInfo.usage = VkBufferUsageFlagBits::VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
		bufferInfo.sharingMode = VkSharingMode::VK_SHARING_MODE_EXCLUSIVE;
		bufferInfo.queueFamilyIndexCount = 0;
		bufferInfo.pQueueFamilyIndices =  0;

		vkCreateBuffer(mDevice, &bufferInfo, nullptr, &mTextureBuffer);

		VkMemoryRequirements req;
		vkGetBufferMemoryRequirements(mDevice, mTextureBuffer, &req);

		VkMemoryAllocateInfo alloc_info = {};
		alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		alloc_info.allocationSize = req.size;
		alloc_info.memoryTypeIndex =
			mMemoryHelper.findMemoryType(VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, req.memoryTypeBits);
		vkAllocateMemory(mDevice, &alloc_info, nullptr, &mTextureMemory);

		vkBindBufferMemory(mDevice, mTextureBuffer, mTextureMemory, 0);
	}

	void VulkanTexture::uploadData()
	{

		VkImageAspectFlags aspectFlags = VkImageAspectFlagBits::VK_IMAGE_ASPECT_COLOR_BIT;

		switch (mTextureType)
		{
		case eTextureResourceType::eTextureResType_1D:
		case eTextureResourceType::eTextureResType_1D_Array:
		case eTextureResourceType::eTextureResType_2D:
		case eTextureResourceType::eTextureResType_3D:
		case eTextureResourceType::eTextureResType_2D_Array:
		case eTextureResourceType::eTextureResType_Cube:
		case eTextureResourceType::eTextureResType_RT_color:
			aspectFlags = VkImageAspectFlagBits::VK_IMAGE_ASPECT_COLOR_BIT;
			break;
		case eTextureResourceType::eTextureResType_RT_depth:
			aspectFlags = VkImageAspectFlagBits::VK_IMAGE_ASPECT_DEPTH_BIT;
			break;
		case eTextureResourceType::eTextureResType_RT_depthStencil:
			aspectFlags = VkImageAspectFlagBits::VK_IMAGE_ASPECT_DEPTH_BIT | VkImageAspectFlagBits::VK_IMAGE_ASPECT_STENCIL_BIT;
			break;
		default:
			break;
		}
		 
		VkBufferImageCopy region;

		region.bufferOffset = 0;
		region.bufferRowLength = 0;
		region.bufferImageHeight = 0;
		region.imageSubresource.aspectMask = aspectFlags;
		region.imageSubresource.mipLevel = 0;
		region.imageSubresource.baseArrayLayer = 0;
		region.imageSubresource.layerCount = 0;
		region.imageOffset.x = 0;
		region.imageOffset.y = 0;
		region.imageOffset.z = 0;
		region.imageExtent.width = mWidth;
		region.imageExtent.height = mHeight;
		region.imageExtent.depth = 1;

		//auto stageBuffer = StagingBuffer::getStagingBuffer()->getCommandBuffer();

		//vkCmdCopyBufferToImage(stageBuffer, mTextureBuffer, mImage, VkImageLayout::VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, 1, &region);
	}

	eTextureResourceType VulkanTexture::getImageType() const
	{
		return mTextureType;
	}

	VkImage VulkanTexture::getImage() const
	{
		return mImage;
	}

	VkImageView VulkanTexture::getImageView() const
	{
		return mImageView;
	}

	VkFormat VulkanTexture::getImageViewFormat() const
	{
		return mFormat;
	}

	void VulkanTexture::createImage()
	{
		VkImageType imageType = VkImageType::VK_IMAGE_TYPE_2D;
		VkImageUsageFlags attachBit = VkImageUsageFlagBits::VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

		switch (mTextureType)
		{
		case eTextureResourceType::eTextureResType_1D:
		case eTextureResourceType::eTextureResType_1D_Array:
			imageType = VkImageType::VK_IMAGE_TYPE_1D;
			break;
		case eTextureResourceType::eTextureResType_RT_color:
			attachBit = attachBit | VkImageUsageFlagBits::VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
		case eTextureResourceType::eTextureResType_RT_depth:
		case eTextureResourceType::eTextureResType_RT_depthStencil:
		case eTextureResourceType::eTextureResType_2D:
			imageType = VkImageType::VK_IMAGE_TYPE_2D;
			break;
		case eTextureResourceType::eTextureResType_3D:
			imageType = VkImageType::VK_IMAGE_TYPE_3D;
			break;
		case eTextureResourceType::eTextureResType_2D_Array:
		case eTextureResourceType::eTextureResType_Cube:
			break;
		default:
			break;
		}

		if (mTextureType == eTextureResourceType::eTextureResType_RT_depth || mTextureType == eTextureResourceType::eTextureResType_RT_depthStencil)
		{
			attachBit = VkImageUsageFlagBits::VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
		}

		VkImageCreateInfo createInfo;

		createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		createInfo.pNext = VK_NULL_HANDLE;
		createInfo.flags = 0;
		createInfo.imageType = imageType;
		createInfo.format = mFormat;
		createInfo.extent.width = mWidth;
		createInfo.extent.height = mHeight;
		createInfo.extent.depth = 1;
		createInfo.mipLevels = 1;
		createInfo.arrayLayers = 1;
		createInfo.samples = VkSampleCountFlagBits::VK_SAMPLE_COUNT_1_BIT;
		createInfo.tiling = VkImageTiling::VK_IMAGE_TILING_OPTIMAL;
		createInfo.usage = VkImageUsageFlagBits::VK_IMAGE_USAGE_SAMPLED_BIT | VkImageUsageFlagBits::VK_IMAGE_USAGE_TRANSFER_DST_BIT | attachBit;
		createInfo.sharingMode = VkSharingMode::VK_SHARING_MODE_EXCLUSIVE;
		createInfo.queueFamilyIndexCount = 0;
		createInfo.pQueueFamilyIndices = 0;
		createInfo.initialLayout = VkImageLayout::VK_IMAGE_LAYOUT_UNDEFINED;

		vkCreateImage(mDevice, &createInfo, nullptr, &mImage);

		createTextureBuffer();
	}

	void VulkanTexture::createImageView()
	{

		VkImageAspectFlags aspectFlags = VkImageAspectFlagBits::VK_IMAGE_ASPECT_COLOR_BIT;
		VkImageViewType imageViewType = VkImageViewType::VK_IMAGE_VIEW_TYPE_1D;

		switch (mTextureType)
		{
		case eTextureResourceType::eTextureResType_1D:
			aspectFlags = VkImageAspectFlagBits::VK_IMAGE_ASPECT_COLOR_BIT;
			imageViewType = VkImageViewType::VK_IMAGE_VIEW_TYPE_1D;
			break;
		case eTextureResourceType::eTextureResType_1D_Array:
			aspectFlags = VkImageAspectFlagBits::VK_IMAGE_ASPECT_COLOR_BIT;
			imageViewType = VkImageViewType::VK_IMAGE_VIEW_TYPE_1D_ARRAY;
			break;
		case eTextureResourceType::eTextureResType_2D:
			aspectFlags = VkImageAspectFlagBits::VK_IMAGE_ASPECT_COLOR_BIT;
			imageViewType = VkImageViewType::VK_IMAGE_VIEW_TYPE_2D;
			break;
		case eTextureResourceType::eTextureResType_3D:
			aspectFlags = VkImageAspectFlagBits::VK_IMAGE_ASPECT_COLOR_BIT;
			imageViewType = VkImageViewType::VK_IMAGE_VIEW_TYPE_3D;
			break;
		case eTextureResourceType::eTextureResType_2D_Array:
			aspectFlags = VkImageAspectFlagBits::VK_IMAGE_ASPECT_COLOR_BIT;
			imageViewType = VkImageViewType::VK_IMAGE_VIEW_TYPE_2D_ARRAY;
			break;
		case eTextureResourceType::eTextureResType_Cube:
			aspectFlags = VkImageAspectFlagBits::VK_IMAGE_ASPECT_COLOR_BIT;
			imageViewType = VkImageViewType::VK_IMAGE_VIEW_TYPE_CUBE;
			break;
		case eTextureResourceType::eTextureResType_RT_color:
			aspectFlags = VkImageAspectFlagBits::VK_IMAGE_ASPECT_COLOR_BIT;
			imageViewType = VkImageViewType::VK_IMAGE_VIEW_TYPE_2D_ARRAY;
			break;
		case eTextureResourceType::eTextureResType_RT_depth:
			aspectFlags = VkImageAspectFlagBits::VK_IMAGE_ASPECT_DEPTH_BIT;
			imageViewType = VkImageViewType::VK_IMAGE_VIEW_TYPE_2D_ARRAY;
			break;
		case eTextureResourceType::eTextureResType_RT_depthStencil:
			aspectFlags = VkImageAspectFlagBits::VK_IMAGE_ASPECT_DEPTH_BIT | VkImageAspectFlagBits::VK_IMAGE_ASPECT_STENCIL_BIT;
			imageViewType = VkImageViewType::VK_IMAGE_VIEW_TYPE_2D_ARRAY;
			break;
		default:
			break;
		}

		VkImageViewCreateInfo createInfo;

		createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		createInfo.pNext = VK_NULL_HANDLE;
		createInfo.flags = 0;
		createInfo.image = mImage;
		createInfo.viewType = imageViewType;
		createInfo.format = mFormat;
		createInfo.components.a = VkComponentSwizzle::VK_COMPONENT_SWIZZLE_A;
		createInfo.components.b = VkComponentSwizzle::VK_COMPONENT_SWIZZLE_B;
		createInfo.components.g = VkComponentSwizzle::VK_COMPONENT_SWIZZLE_G;
		createInfo.components.r = VkComponentSwizzle::VK_COMPONENT_SWIZZLE_R;
		createInfo.subresourceRange.aspectMask = aspectFlags;
		createInfo.subresourceRange.baseMipLevel = 0;
		createInfo.subresourceRange.levelCount = 1;
		createInfo.subresourceRange.baseArrayLayer = 0;
		createInfo.subresourceRange.layerCount = 1;

		vkCreateImageView(mDevice, &createInfo, nullptr, &mImageView);

	}

	void VulkanTexture::createTextureBuffer()
	{
		VkMemoryAllocateInfo allocInfo;

		VkMemoryRequirements memReq;

		vkGetImageMemoryRequirements(mDevice, mImage, &memReq);

		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.pNext = VK_NULL_HANDLE;
		allocInfo.allocationSize = memReq.size;
		uint32_t typeIndex = mMemoryHelper.findMemoryType(VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, memReq.memoryTypeBits);
		allocInfo.memoryTypeIndex = typeIndex;

		vkAllocateMemory(mDevice, &allocInfo, nullptr, &mTextureMemory);

		vkBindImageMemory(mDevice, mImage, mTextureMemory, 0);
	}

	/*

	VulkanTexture2D

	*/

	VulkanTexture2D::VulkanTexture2D(VkDevice device, VulkanMemoryHelper& memHelper, eTextureDataFormat dataFormat)
		: mDevice(device)
		, mMemoryHelper(memHelper)
		, mImage(VK_NULL_HANDLE)
		, mImageView(VK_NULL_HANDLE)
		, mWidth(0U)
		, mHeight(0U)
		, mFormat(VkFormat::VK_FORMAT_UNDEFINED)
		, mTextureBuffer(VK_NULL_HANDLE)
		, mTextureMemory(VK_NULL_HANDLE)
	{
		dataFormat;
	}

	VulkanTexture2D::~VulkanTexture2D() {
		if (mImage != VK_NULL_HANDLE)
		{
			vkDestroyImage(mDevice, mImage, nullptr);
		}
		if (mImageView != VK_NULL_HANDLE)
		{
			vkDestroyImageView(mDevice, mImageView, nullptr);
		}
		if (mTextureBuffer != VK_NULL_HANDLE)
		{
			vkDestroyBuffer(mDevice, mTextureBuffer, nullptr);
		}
		if (mTextureMemory != VK_NULL_HANDLE)
		{
			vkFreeMemory(mDevice, mTextureMemory, nullptr);
		}
	}

	const uint32_t VulkanTexture2D::getWidth() const 
	{
		return mWidth;
	}
	const uint32_t VulkanTexture2D::getHeight() const
	{
		return mHeight;
	}

	void VulkanTexture2D::setTextureData(uint32_t width, uint32_t height, void* data)
	{
		width;
		height;
		data;
	}

}

