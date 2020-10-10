
#pragma warning(push)
#pragma warning(disable : 4127)
#pragma warning(disable : 4251)
#define GLM_FORCE_INLINE 
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_transform.hpp>
#pragma warning(pop)

#include <swizzle/gfxutils/Camera.hpp>

namespace swizzle
{

    OrthoCamera::OrthoCamera(float left, float right, float bottom, float top)
        : mProjectionMatrix(1.0F), mViewMatrix(1.0F), mProjView(1.0F)
		, mAspects(left, right, bottom, top), mPosition(0.0F), mScale(1.0F)
    {
		mProjectionMatrix = glm::ortho(mAspects.x, mAspects.y, mAspects.z, mAspects.w, -1.0F, 1.0F);
		recalculateViewProj();
    }

    OrthoCamera::~OrthoCamera() {}
	
    void OrthoCamera::setPosition(glm::vec3 pos)
    {
        mPosition = pos;
        recalculateViewProj();
    }

	void OrthoCamera::setScale(float_t scale)
	{
		mScale = glm::clamp(scale, 0.01F, 10.0F);
		glm::vec4 s = mScale * mAspects;
		mProjectionMatrix = glm::ortho(s.x, s.y, s.z, s.w, -1.0F, 1.0F);
		recalculateViewProj();
	}

	const glm::vec3& OrthoCamera::getPosition()
    {
        return *&mPosition;
    }

    const glm::mat4& OrthoCamera::getViewProjection() const
    {
		return mProjView;
    }

    void OrthoCamera::recalculateViewProj()
    {
        glm::mat4 transform = glm::translate(glm::mat4(1.0f), mPosition);

        mViewMatrix = glm::inverse(transform);
	    mProjView = mProjectionMatrix * mViewMatrix;
    }

} // namespace swizzle