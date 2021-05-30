
#include "CameraController.hpp"
#include <swizzle/core/Input.hpp>
#include <glm/glm.hpp>

CameraController::CameraController(PerspectiveCamera& cam)
    : mCamera(cam)
    , mPosition(0.0F)
    , mDirection(0.0F)
    , mStartDir(-1.0F, 0.0F, 0.0F)
    , mAngleH(0.0F)
    , mAngleV(0.0F)
    , mOldX(0.0F)
    , mOldY(0.0F)
{
    mCamera.lookAt(mPosition, mPosition + mDirection);
    mousePan(0, 0);
}

CameraController::~CameraController()
{
}

void CameraController::update(float dt)
{
    dt;
    mPosition = mCamera.getPosition();

    //float dx, dy;

    //swizzle::input::GetMouseDelta(dx, dy);
    //if (swizzle::input::IsMouseButtonPressed(swizzle::input::Mouse::LeftClick))
    //{
    //    dx *= 0.1F;
    //    dy *= 0.1F;

    //    mousePan(dx, dy);
    //}
    //keyPan(dt);

    mCamera.lookAt(mPosition, mPosition + mDirection);
}

void CameraController::keyPan(float dt)
{
    const float_t speed = 5.0F * dt;

    if (swizzle::input::IsKeyPressed(swizzle::input::Keys::KeyW))
    {
        mPosition += mDirection * speed;
    }
    if (swizzle::input::IsKeyPressed(swizzle::input::Keys::KeyS))
    {
        mPosition -= mDirection * speed;
    }

    if (swizzle::input::IsKeyPressed(swizzle::input::Keys::KeyA))
    {
        glm::vec3 left = cross(glm::vec3(0.0F, 1.0F, 0.0F), mDirection);
        left = glm::normalize(left);
        mPosition += left * speed;
    }
    if (swizzle::input::IsKeyPressed(swizzle::input::Keys::KeyD))
    {
        glm::vec3 left = cross(mDirection, glm::vec3(0.0F, 1.0F, 0.0F));
        left = glm::normalize(left);
        mPosition += left * speed;
    }

    mCamera.setPosition(mPosition);
}

void CameraController::mousePan(float dx, float dy)
{
    mAngleH -= dx;
    mAngleV -= dy;
    if (mAngleV > 89)
    {
        mAngleV = 89;
    }
    if (mAngleV < -89)
    {
        mAngleV = -89;
    }

    glm::vec3 dir = mStartDir;

    F32 rotateRad = glm::radians(mAngleV);

    glm::mat3 rotV = glm::mat3(glm::cos(rotateRad), -glm::sin(rotateRad), 0.0f,
                               glm::sin(rotateRad), glm::cos(rotateRad), 0.0f,
                               0.0f, 0.0f, 1.0f);

    dir = rotV * dir;
    dir = glm::normalize(dir);

    rotateRad = glm::radians(mAngleH);
    glm::mat3 rotH = glm::mat3(glm::cos(rotateRad), 0.0f, -glm::sin(rotateRad),
                               0.0f, 1.0f, 0.0f,
                               glm::sin(rotateRad), 0.0f, glm::cos(rotateRad));

    dir = rotH * dir;
    mDirection = glm::normalize(dir);
}