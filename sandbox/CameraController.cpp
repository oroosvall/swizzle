
#include "CameraController.hpp"
#include <swizzle/input/Input.hpp>
#include <glm/glm.hpp>

CameraController::CameraController(PerspectiveCamera& cam)
    : mCamera(cam)
    , mPosition(0.0F)
    , mDirection(0.0F)
    , mStartDir(-1.0F, 0.0F, 0.0F)
    , mAngleV(0.0F)
    , mAngleH(0.0F)
    , mOldX(0.0F)
    , mOldY(0.0F)
{
    mCamera.lookAt(mPosition, mPosition + mDirection);
}

CameraController::~CameraController()
{

}

void CameraController::update(float dt)
{
    mPosition = mCamera.getPosition();

    //if (swizzle::input::IsKeyPressed(swizzle::input::Keys::KeyQ))
    //{
    //    mAngleH += 10.0F * dt;
    //}
    //if (swizzle::input::IsKeyPressed(swizzle::input::Keys::KeyE))
    //{
    //    mAngleH -= 10.0F * dt;
    //    auto px = mPosition.x;
    //    auto py = mPosition.y;
    //    auto pz = mPosition.z;
    //    
    //    auto dx = mDirection.x;
    //    auto dy = mDirection.y;
    //    auto dz = mDirection.z;
    //    printf("(%f, %f, %f), (%f, %f, %f)\n", px, py, pz, dx, dy, dz);
    //}

    float dx, dy;

    swizzle::input::GetMouseDelta(dx, dy);
    if (swizzle::input::IsMouseButtonPressed(swizzle::input::Mouse::LeftClick))
    {
        dx /= 10.0F;
        dy /= 10.0F;
        mousePan(dx, dy);
        //if( dx != 0.0 || dy != 0.0)
        //    printf("%f, %f\n", dx, dy);
        /*auto px = mPosition.x;
        auto py = mPosition.y;
        auto pz = mPosition.z;
            
        auto dx = mDirection.x;
        auto dy = mDirection.y;
        auto dz = mDirection.z;
        printf("(%f, %f, %f), (%f, %f, %f)\n", px, py, pz, dx, dy, dz);*/
    }
    keyPan(dt);

    mCamera.lookAt(mPosition, mPosition + mDirection);
}

void CameraController::keyPan(float dt)
{
    const float_t speed = 5.0F * dt;

    glm::vec3 pos = mCamera.getPosition();

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
    mAngleH -= dx; // * (mouseSpeed / 5.0f * playbackSpeed);
    mAngleV -= dy; // * (mouseSpeed / 5.0f * playbackSpeed);
    if (mAngleV > 89)
        mAngleV = 89;
    if (mAngleV < -89)
        mAngleV = -89;

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