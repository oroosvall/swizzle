#ifndef PLAYER_HPP
#define PLAYER_HPP

#include <common/Common.hpp>

#include <swizzle/Swizzle.hpp>
#include <swizzle/asset/MeshLoader.hpp>

#include <glm/glm.hpp>

class Player
{
public:
    Player();
    virtual ~Player();

    void initResources(common::Resource<sw::gfx::GfxContext> ctx);
    void cleanupResources();

    void update(F32 dt);

    SwBool shouldShoot();

    glm::mat4 getMatrix();
    glm::vec3 getPosition();

    common::Resource<sw::gfx::Buffer> getVertexBuffer();
    common::Resource<sw::gfx::Buffer> getIndexBuffer();

private:

    glm::vec3 mPlayerPos;

    sw::Mesh mPlayerShip;

    F32 moveSpeed = 20.0F;
    F32 mCooldown = 0.0F;
    F32 mReloadSpeed = 0.5F;
};

#endif