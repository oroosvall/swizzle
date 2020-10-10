#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <swizzle/Api.hpp>
#include <swizzle/gfxutils/Vectors.hpp>
#include <swizzle/gfxutils/Matrix.hpp>

#pragma warning(push)
#pragma warning(disable : 4127)
#pragma warning(disable : 4251)
#define GLM_FORCE_INLINE 
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

namespace swizzle
{
	class SWIZZLE_API OrthoCamera
	{
	public:

		OrthoCamera(float left, float right, float bottom, float top);
		~OrthoCamera();
		
		void setPosition(glm::vec3 pos);
		void setScale(float_t scale);
		const glm::vec3& getPosition();

		const glm::mat4& getViewProjection() const;

	private:

		void recalculateViewProj();

		glm::mat4 mProjectionMatrix;
		glm::mat4 mViewMatrix;
		glm::mat4 mProjView;

		glm::vec4 mAspects;
		glm::vec3 mPosition;
		float_t mScale;

	};

	class SWIZZLE_API PerspectiveCamera
	{
	public:

		PerspectiveCamera(float fov, float width, float heigth);
		~PerspectiveCamera();

		void changeFov(float fov);
		void changeAspect(float width, float height);

		void setPosition(glm::vec3 pos);
		void setRotation(glm::vec3 rot);

		const glm::vec3& getPosition();

		const glm::mat4& getViewProjection() const;

	private:

		void recalculatePerspective();
		void recalculateViewProj();

		float mFov;
		float mWidth;
		float mHeight;

		glm::mat4 mProjectionMatrix;
		glm::mat4 mViewMatrix;
		glm::mat4 mProjView;

		glm::vec3 mPosition;
		glm::vec3 mRotation;

	};

}

#pragma warning(pop)

#endif