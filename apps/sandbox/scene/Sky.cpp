
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

Sky::Sky(common::Resource<swizzle::gfx::Buffer> mesh, common::Resource<swizzle::gfx::Texture> texture, common::Resource<swizzle::gfx::Material> material, common::Resource<swizzle::gfx::Shader> shader)
    : mMesh(mesh)
    , mTexture(texture)
    , mMaterial(material)
    , mShader(shader)
{

}

void Sky::update(DeltaTime dt, common::Resource<swizzle::gfx::CommandBuffer> cmd)
{
    UNUSED_ARG(dt);
    cmd->uploadTexture(mTexture);
}

void Sky::render(common::Resource<swizzle::gfx::CommandBuffer> cmd, PerspectiveCamera& cam)
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
    t.view = cam.getView();
    t.proj = cam.getProjection();
    t.eye = glm::vec4(cam.getPosition(), 1.0F);

    cmd->bindShader(mShader);

    cmd->bindMaterial(mShader, mMaterial);
    cmd->setViewport(U32(x), U32(y));

    cmd->setShaderConstant(mShader, (U8*)&t, sizeof(t));

    cmd->draw(mMesh);

}

/* Class Protected Function Definition */

/* Class Private Function Definition */
