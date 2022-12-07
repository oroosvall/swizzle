
/* Include files */

#include "ParticleSystem.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <swizzle/core/Input.hpp>

/* Defines */

/* Typedefs */

/* Structs/Classes */

/* Static Variables */

/* Static Function Declaration */

/* Static Function Definition */

/* Function Definition */

/* Class Public Function Definition */

ParticleSystem::ParticleSystem(common::Resource<swizzle::gfx::GfxContext> ctx, U32 particleCount, glm::vec3 startPos,
                               glm::vec3 dir, common::Resource<swizzle::gfx::Texture> texture,
                               common::Resource<swizzle::gfx::Shader> shader)
    : mParticleBuffer(nullptr)
    , mTexture(texture)
    , mMaterial(nullptr)
    , mShader(shader)
    , mParticlePositions()
    , mNumParticles(particleCount)
    , mActiveParticles(0ull)
    , mParticles()
    , mStartPos(startPos)
    , mDirection(dir)
    , rd()
    , mEnabled(true)
{
    mParticleBuffer = ctx->createBuffer(swizzle::gfx::BufferType::Vertex);

    mParticlePositions.resize(mNumParticles);
    mParticles.resize(mNumParticles);

    mParticleBuffer->setBufferData(mParticlePositions.data(), sizeof(glm::vec3) * mParticlePositions.size(),
                                   sizeof(glm::vec3));

    mMaterial = ctx->createMaterial(mShader, swizzle::gfx::SamplerMode::SamplerModeRepeat);
    mMaterial->setDescriptorTextureResource(0u, mTexture);
}

void ParticleSystem::update(DeltaTime dt, SceneRenderSettings& settings,
                            common::Unique<swizzle::gfx::CommandTransaction>& trans)
{
    if (settings.mParticles != mEnabled)
    {
        mEnabled = settings.mParticles;
    }

    if (!mEnabled)
    {
        return;
    }

    trans->uploadTexture(mTexture);

    for (U32 i = 0; i < mActiveParticles; ++i)
    {
        auto& p = mParticles[i];
        if (p.mTtl < 0.0)
        {
            p = spawnParticle();
        }
        p.mVel -= glm::vec3(0, 9.82, 0) * dt; // gravity
        mParticlePositions[i] = p.mPos = (p.mPos + p.mVel * dt);
        p.mTtl -= dt;
    }

    if (mActiveParticles < mNumParticles)
    {
        mParticles[mActiveParticles] = spawnParticle();
        mParticlePositions[mActiveParticles] = mParticles[mActiveParticles].mPos;
        mActiveParticles++;
    }

    mParticleBuffer->setBufferData(mParticlePositions.data(), sizeof(glm::vec3) * mParticlePositions.size(),
                                   sizeof(glm::vec3));

}

void ParticleSystem::render(common::Unique<swizzle::gfx::DrawCommandTransaction>& trans, PerspectiveCamera& cam)
{
    if (!mEnabled)
    {
        return;
    }

    struct tmp
    {
        glm::mat4 viewProj;
        glm::vec4 eye;
    };

    float x, y;

    cam.getCameraSize(x, y);

    tmp t = {};
    t.viewProj = cam.getProjection() * cam.getView();
    t.eye = glm::vec4(cam.getPosition(), 1.0F);

    trans->bindShader(mShader);

    trans->bindMaterial(mShader, mMaterial);
    trans->setViewport(U32(x), U32(y));

    trans->setShaderConstant(mShader, (U8*)&t, sizeof(t));

    trans->bindVertexBuffer(mParticleBuffer);
    trans->drawNoBind(mActiveParticles, 0u);

    //trans->draw(mParticleBuffer);
}

/* Class Protected Function Definition */

/* Class Private Function Definition */

Particle ParticleSystem::spawnParticle()
{
    std::uniform_real_distribution<F32> dist(-1.0f, 1.0f);
    std::uniform_real_distribution<F32> dist2(5.0f, 7.0f);
    std::uniform_real_distribution<F32> dist3(2.0f, 3.0f);

    Particle p = {};

    glm::vec3 dir = mDirection;
    dir.x += dist(rd) * 0.1f;
    dir.y += dist(rd) * 0.1f;
    dir.z += dist(rd) * 0.1f;
    dir = glm::normalize(dir);

    p.mPos = mStartPos;
    p.mVel = dir * dist2(rd);
    p.mTtl = dist3(rd);

    return p;
}