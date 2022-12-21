
/* Include files */

#include "Sky.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

/* Defines */

/* Typedefs */

/* Structs/Classes */

/* Static Variables */

/* Static Function Declaration */

/* Static Function Definition */

/* Function Definition */

/* Class Public Function Definition */

Sky::Sky(common::Resource<swizzle::gfx::Buffer> mesh, common::Resource<swizzle::gfx::Material> material,
         common::Resource<swizzle::gfx::Shader> shader, common::Resource<swizzle::gfx::Buffer> uniform,
         common::Resource<swizzle::gfx::Buffer> inst)
    : mMesh(mesh)
    , mMaterial(material)
    , mShader(shader)
    , mUniformBuffer(uniform)
    , mInst(inst)
{
}

void Sky::update(DeltaTime dt, SceneRenderSettings& settings, common::Unique<swizzle::gfx::CommandTransaction>& trans)
{
    UNUSED_ARG(dt);
    UNUSED_ARG(settings);
    UNUSED_ARG(trans);
    //trans->uploadTexture(mTexture);
    mUniformBuffer->setBufferData(&settings.mSkyInfo, sizeof(SkyInfo), sizeof(SkyInfo));
    glm::mat4 pos = glm::translate(glm::mat4(1.0F), {settings.mSkyInfo.mCameraEye});
    mInst->setBufferData(&pos, sizeof(glm::mat4), sizeof(glm::mat4));

    mMaterial->setDescriptorBufferResource(0u, mUniformBuffer, sizeof(SkyInfo));
}

void Sky::render(common::Unique<swizzle::gfx::DrawCommandTransaction>& trans, OrthoCamera& cam)
{
    UNUSED_ARG(trans);
    UNUSED_ARG(cam);
}

void Sky::render(common::Unique<swizzle::gfx::DrawCommandTransaction>& trans, PerspectiveCamera& cam)
{
    struct tmp
    {
        glm::mat4 model;
        glm::mat4 view;
        glm::mat4 proj;
        glm::vec4 eye;
    };

    float x, y;

    cam.getCameraSize(x, y);

    tmp t = {};
    t.model = glm::translate(glm::mat4(1.0F), {cam.getPosition()});
    t.view = cam.getView();
    t.proj = cam.getProjection();
    t.eye = glm::vec4(cam.getPosition(), 1.0F);

    trans->bindShader(mShader);

    trans->bindMaterial(mShader, mMaterial);
    trans->setViewport(U32(x), U32(y));
    trans->enableStencilTest(false);

    trans->setShaderConstant(mShader, (U8*)&t, sizeof(t));

    trans->drawInstanced(mMesh, mInst);
}

void Sky::renderMirrorTransform(common::Unique<swizzle::gfx::DrawCommandTransaction>& trans, PerspectiveCamera& cam, glm::mat4& mat)
{
    struct tmp
    {
        glm::mat4 model;
        glm::mat4 view;
        glm::mat4 proj;
        glm::vec4 eye;
    };

    float x, y;

    cam.getCameraSize(x, y);

    tmp t = {};
    t.model = glm::translate(glm::mat4(1.0F), { cam.getPosition() });
    t.view = cam.getView() * mat;
    t.proj = cam.getProjection();
    t.eye = glm::vec4(cam.getPosition(), 1.0F);

    trans->bindShader(mShader);

    trans->bindMaterial(mShader, mMaterial);
    trans->setViewport(U32(x), U32(y));
    trans->enableStencilTest(true);

    trans->setShaderConstant(mShader, (U8*)&t, sizeof(t));

    trans->drawInstanced(mMesh, mInst);
    trans->enableStencilTest(false);
}

/* Class Protected Function Definition */

/* Class Private Function Definition */
