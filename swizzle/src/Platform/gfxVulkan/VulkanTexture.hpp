#ifndef VULKAN_TEXTURE_HPP
#define VULKAN_TEXTURE_HPP

#include <swizzle/gfx/Texture.hpp>

#include "vk.hpp"
#include "VulkanMemoryHelper.hpp"

namespace swizzle
{

	enum class eTextureResourceType
	{
		eTextureResType_1D,
		eTextureResType_1D_Array,
		eTextureResType_2D,
		eTextureResType_3D,
		eTextureResType_2D_Array,
		eTextureResType_Cube,

		eTextureResType_RT_color,
		eTextureResType_RT_depth,
	};

	struct TextureInfo
	{
		VkDevice device;
		VulkanMemoryHelper& memHelper;
		uint32_t width;
		uint32_t height;
		eTextureResourceType resourceType;
		VkFormat format;
		bool createFromExternal;
		VkImage exernalImages;
		VkImageView externalView;
	};

	class VulkanTexture : public Texture2D
	{
	public:
		VulkanTexture(const TextureInfo& textureInfo);
		virtual ~VulkanTexture();

		//virtual eRenderApi getResourceApiType() const override;

		virtual const uint32_t getWidth() const override;
		virtual const uint32_t getHeight() const override;
		virtual uint32_t getLayer() const ;

		virtual void setTextureData(uint32_t width, uint32_t height, void* data) override;
		virtual void setTextureData(void* data, uint32_t width, uint32_t height, uint32_t layer) ;
		virtual void uploadData() ;

		eTextureResourceType getImageType() const;
		VkImage getImage() const;
		VkImageView getImageView() const;
		VkFormat getImageViewFormat() const;

	private:

		void createImage();
		void createImageView();

		void createTextureBuffer();

		bool mOwnImage;
		VkDevice mDevice;
		VulkanMemoryHelper& mMemoryHelper;

		eTextureResourceType mTextureType;

		VkImage mImage;
		VkImageView mImageView;
		
		uint32_t mWidth;
		uint32_t mHeight;

		VkFormat mFormat;

		VkBuffer mTextureBuffer;
		VkDeviceMemory mTextureMemory;

	};

	class VulkanTexture2D : public Texture2D
	{
	public:
		VulkanTexture2D(VkDevice device, VulkanMemoryHelper& memHelper, eTextureDataFormat dataFormat);
		virtual ~VulkanTexture2D();

		virtual const uint32_t getWidth() const override;
		virtual const uint32_t getHeight() const override;

		virtual void setTextureData(uint32_t width, uint32_t height, void* data) override;

	private:
		VkDevice mDevice;
		VulkanMemoryHelper& mMemoryHelper;

		VkImage mImage;
		VkImageView mImageView;

		uint32_t mWidth;
		uint32_t mHeight;

		VkFormat mFormat;

		VkBuffer mTextureBuffer;
		VkDeviceMemory mTextureMemory;
	};

}

#endif