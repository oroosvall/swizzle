
#pragma warning(push)
#pragma warning(disable : 4127)
#pragma warning(disable : 4251)
#define GLM_FORCE_INLINE 
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#pragma warning(pop)

#include <swizzle/gfxutils/Camera.hpp>

namespace swizzle
{

    PerspectiveCamera::PerspectiveCamera(float fov, float width, float heigth)
        : mProjectionMatrix(glm::perspective(fov, width/heigth, 0.01F, 10.0F)), mViewMatrix(1.0F), mProjView(1.0F), mPosition(0.0F), mRotation(0.0F)
    {
		//mProjectionMatrix[1][1] *= -1;
		recalculateViewProj();
    }

	PerspectiveCamera::~PerspectiveCamera() {}
	
    void PerspectiveCamera::setPosition(glm::vec3 pos)
    {
        mPosition = pos;
        recalculateViewProj();
    }

	void PerspectiveCamera::setRotation(glm::vec3 rot)
	{
		mRotation = rot;
		recalculateViewProj();
	}

	const glm::vec3& PerspectiveCamera::getPosition()
    {
        return *&mPosition;
    }

    const glm::mat4& PerspectiveCamera::getViewProjection() const
    {
		return mProjView;
    }

    void PerspectiveCamera::recalculateViewProj()
    {
        glm::mat4 transform = glm::translate(glm::mat4(1.0f), mPosition);

        mViewMatrix = glm::inverse(transform);
	    mProjView = mProjectionMatrix * mViewMatrix;
    }

} // namespace swizzle