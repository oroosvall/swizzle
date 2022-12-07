#ifndef PARTICLE_SYSTEM_HPP
#define PARTICLE_SYSTEM_HPP

/* Include files */

#include "Renderable.hpp"

#include <swizzle/asset2/Assets.hpp>
#include <swizzle/gfx/Context.hpp>

#include <random>

/* Defines */

/* Typedefs/enums */

/* Forward Declared Structs/Classes */

/* Struct Declaration */

struct Particle
{
    glm::vec3 mPos;
    glm::vec3 mVel;
    F32 mTtl;
};

/* Class Declaration */

class ParticleSystem : public Renderable
{
public:
    ParticleSystem(common::Resource<swizzle::gfx::GfxContext> ctx, U32 particleCount, glm::vec3 startPos, glm::vec3 dir,
                   common::Resource<swizzle::gfx::Texture> texture, common::Resource<swizzle::gfx::Shader> shader);

    virtual void update(DeltaTime dt, SceneRenderSettings& settings,
                        common::Unique<swizzle::gfx::CommandTransaction>& trans) override;
    virtual void render(common::Unique<swizzle::gfx::DrawCommandTransaction>& trans, PerspectiveCamera& cam) override;

private:
    Particle spawnParticle();

    common::Resource<swizzle::gfx::Buffer> mParticleBuffer;
    common::Resource<swizzle::gfx::Texture> mTexture;
    common::Resource<swizzle::gfx::Material> mMaterial;
    common::Resource<swizzle::gfx::Shader> mShader;

    std::vector<glm::vec3> mParticlePositions; // this contains the result of updated particles;
    U32 mNumParticles;
    U32 mActiveParticles;
    glm::vec3 mStartPos;
    glm::vec3 mDirection;
    std::vector<Particle> mParticles;

    std::random_device rd;

    SwBool mEnabled;
};

/* Function Declaration */

#endif
