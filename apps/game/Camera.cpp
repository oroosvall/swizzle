
#if defined(_WIN32)
#pragma warning(push)
#pragma warning(disable : 4127)
#pragma warning(disable : 4251)
#pragma warning(disable : 4201)
#endif
#define GLM_FORCE_INLINE
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>
#if defined(_WIN32)
#pragma warning(pop)
#endif

#include "Camera.hpp"

PerspectiveCamera::PerspectiveCamera(float fov, float width, float heigth)
    : mFov(fov)
    , mWidth(width)
    , mHeight(heigth)
    , mProjectionMatrix()
    , mViewMatrix(1.0F)
    , mProjView(1.0F)
    , mPosition(0.0F)
    , mRotation(0.0F)
{
    // mProjectionMatrix[1][1] *= -1;
    recalculatePerspective();
    recalculateViewProj();
}

PerspectiveCamera::~PerspectiveCamera() {}

void PerspectiveCamera::changeFov(float fov)
{
    mFov = fov;
    recalculatePerspective();
}

void PerspectiveCamera::changeAspect(float width, float height)
{
    if ((width != 0.0F) && (height != 0.0F))
    {
        mWidth = width;
        mHeight = height;
        recalculatePerspective();
    }
}

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

void PerspectiveCamera::lookAt(glm::vec3 pos, glm::vec3 dir, glm::vec3 up)
{
    mViewMatrix = glm::lookAt(pos, dir, up);
}

const glm::vec3& PerspectiveCamera::getPosition()
{
    return *&mPosition;
}

const glm::vec3& PerspectiveCamera::getRotation()
{
    return *&mRotation;
}

const glm::mat4& PerspectiveCamera::getView() const
{
    return mViewMatrix;
}

const glm::mat4& PerspectiveCamera::getProjection() const
{
    return mProjectionMatrix;
}

const glm::mat4& PerspectiveCamera::getViewProjection() const
{
    return mProjView;
}

void PerspectiveCamera::recalculatePerspective()
{
    mProjectionMatrix = glm::perspective(mFov, mWidth / mHeight, 0.01F, 100.0F);
}

void PerspectiveCamera::recalculateViewProj()
{
    glm::mat4 transform = glm::translate(glm::mat4(1.0f), mPosition);
    glm::mat4 rot = glm::yawPitchRoll(mRotation.z, mRotation.x, mRotation.y);

    mViewMatrix = glm::inverse(transform * rot);
    mProjView = mProjectionMatrix * mViewMatrix;
}
