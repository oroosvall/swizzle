#include "player.hpp"

#include <glm/gtc/matrix_transform.hpp>

Player::Player()
    : mPlayerPos()
    , mPlayerShip()
{

}

Player::~Player()
{

}

void Player::initResources(common::Resource<sw::gfx::GfxContext> ctx)
{
    mPlayerShip = sw::asset::LoadMesh(ctx, "meshes/game/ship.obj", true);
}

void Player::cleanupResources()
{
    mPlayerShip = {};
}

void Player::update(F32 dt)
{
    if (mCooldown > 0.0F)
    {
        mCooldown -= dt;
    }

    if (sw::input::IsKeyPressed(sw::input::Keys::KeyW))
    {
        mPlayerPos.y += moveSpeed * dt;
    }
    if (sw::input::IsKeyPressed(sw::input::Keys::KeyA))
    {
        mPlayerPos.z += moveSpeed * dt;
    }
    if (sw::input::IsKeyPressed(sw::input::Keys::KeyS))
    {
        mPlayerPos.y -= moveSpeed * dt;
    }
    if (sw::input::IsKeyPressed(sw::input::Keys::KeyD))
    {
        mPlayerPos.z -= moveSpeed * dt;
    }
}

SwBool Player::shouldShoot()
{
    SwBool shoot = false;
    if (sw::input::IsKeyPressed(sw::input::Keys::KeySpace))
    {
        if (mCooldown <= 0.0F)
        {
            mCooldown = mReloadSpeed;
            shoot = true;
        }
    }

    return shoot;
}

glm::mat4 Player::getMatrix()
{
    return glm::translate(glm::mat4(1.0F), mPlayerPos);
}

glm::vec3 Player::getPosition()
{
    return mPlayerPos;
}

common::Resource<sw::gfx::Buffer> Player::getBuffer()
{
    return mPlayerShip.mVertexBuffer;
}