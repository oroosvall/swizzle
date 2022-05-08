
/* Include files */

#include "Scene.hpp"

#include <iostream>
#include <fstream>

#include "Sky.hpp"
#include "Animated.hpp"

/* Defines */

/* Typedefs */

/* Structs/Classes */

/* Static Variables */

/* Static Function Declaration */

/* Static Function Definition */

/* Function Definition */

/* Class Public Function Definition */

Scene::Scene(common::Resource<AssetManager> assetManager, common::Resource<Compositor> compositor)
    : mAssetManager(assetManager)
    , mCompositor(compositor)
    , mSceneState(SceneState::NotLoaded)
{

}

Scene::~Scene()
{

}

void Scene::clear()
{
    mAssetManager->clear();
}

SwBool Scene::loadScene(std::string file)
{
    SwBool ok = false;
    std::ifstream inFile(file, std::ios::binary);

    if (inFile.is_open())
    {
        SwChar magic[4] = {};
        inFile.read(magic, sizeof(magic));

        if (strncmp(magic, "SC A", 4) == 0)
        {
            std::string str;
            std::getline(inFile, str);
            ok = loadAscii(inFile);
        }
        else if (strncmp(magic, "SC B", 4) == 0)
        {
            ok = loadBinary(inFile);
        }
        else
        {
            ok = false;
        }
    }
    if (ok)
    {
        mSceneState = SceneState::LoadingShowProgress;
    }

    return ok;
}

void Scene::loadSky()
{
    swizzle::Mesh skysphere;
    common::Resource<swizzle::gfx::Shader> skyShader;
    common::Resource<swizzle::gfx::Texture> skyTexture;
    common::Resource<swizzle::gfx::Material> skyMaterial;

    swizzle::gfx::ShaderAttributeList attribsSky = {};
    attribsSky.mBufferInput = {
        { swizzle::gfx::ShaderBufferInputRate::InputRate_Vertex, sizeof(float) * (3U + 3U + 2U) }
    };
    attribsSky.mAttributes = {
        { 0U, swizzle::gfx::ShaderAttributeDataType::vec3f, 0U},
        { 0U, swizzle::gfx::ShaderAttributeDataType::vec3f, sizeof(float) * 3U },
        { 0U, swizzle::gfx::ShaderAttributeDataType::vec2f, sizeof(float) * 6U }
    };
    attribsSky.mEnableDepthTest = false;
    attribsSky.mEnableBlending = false;

    skyShader = mCompositor->createShader(0u, attribsSky);
    skyShader->load("shaders/sky.shader");

    skyMaterial = skyShader->createMaterial();

    skyTexture = mAssetManager->loadCubeTexture(
        "texture/right.png", "texture/left.png", "texture/top.png", "texture/bottom.png", "texture/front.png", "texture/back.png");

    skyMaterial->setDescriptorTextureResource(0, skyTexture);

    skysphere = mAssetManager->loadMesh2("meshes/inverted_sphere.obj");

    mRenderables.emplace_back(common::CreateRef<Sky>(skysphere.mVertexBuffer, skyTexture, skyMaterial, skyShader));

}

void Scene::loadAnimMesh()
{
    swizzle::MeshAnimated meshAnimated = mAssetManager->loadAnimMesh("meshes/test.swm");
    common::Resource<swizzle::gfx::Shader> shader;
    common::Resource<swizzle::gfx::Texture> texture;
    common::Resource<swizzle::gfx::Material> material;

    swizzle::gfx::ShaderAttributeList attribsAnim = {};
    attribsAnim.mBufferInput = {
        { swizzle::gfx::ShaderBufferInputRate::InputRate_Vertex, sizeof(float) * (3u + 3u + 2u + 4u + 4u) }
    };
    attribsAnim.mAttributes = {
        { 0u, swizzle::gfx::ShaderAttributeDataType::vec3f, 0u},
        { 0u, swizzle::gfx::ShaderAttributeDataType::vec3f, sizeof(float) * 3u },
        { 0u, swizzle::gfx::ShaderAttributeDataType::vec2f, sizeof(float) * 6u },
        { 0u, swizzle::gfx::ShaderAttributeDataType::vec4u, sizeof(float) * 8u },
        { 0u, swizzle::gfx::ShaderAttributeDataType::vec4f, sizeof(float) * 12u }
    };
    attribsAnim.mEnableDepthTest = true;
    attribsAnim.mEnableBlending = false;

    texture = mAssetManager->loadTexture("texture/lightGray.png");

    shader = mCompositor->createShader(0u, attribsAnim);
    shader->load("shaders/animated.shader");
    material = shader->createMaterial();
    material->setDescriptorTextureResource(0u, texture);
    material->setDescriptorBufferResource(1u, meshAnimated.mBoneBuffer, ~0ull);

    mRenderables.emplace_back(common::CreateRef<Animated>( meshAnimated.mVertexBuffer, meshAnimated.mIndexBuffer, meshAnimated.mBoneBuffer, texture, material, shader));
}

SceneState Scene::update(DeltaTime dt, common::Resource<swizzle::gfx::CommandBuffer> cmdBuf)
{

    for (auto& it : mRenderables)
    {
        it->update(dt, cmdBuf);
    }

    return mSceneState;
}

void Scene::render(common::Resource<swizzle::gfx::CommandBuffer> cmdBuf, PerspectiveCamera& cam)
{
    for (auto& it : mRenderables)
    {
        it->render(cmdBuf, cam);
    }

}

/* Class Protected Function Definition */

/* Class Private Function Definition */

SwBool Scene::loadAscii(std::ifstream& stream)
{
    SwBool ok = true;
    std::string str;
    std::getline(stream, str);

    U32 meshCount = strtoul(str.c_str(), nullptr, 10u);
    for (U32 i = 0u; i < meshCount; ++i)
    {
        std::getline(stream, str);

        str.erase(std::remove(str.begin(), str.end(), '\r'), str.end());

        if (!mAssetManager->loadMesh(i, str.c_str()))
        {
            ok = false;
            break;
        }
    }

    if (ok)
    {
        std::getline(stream, str);
        U32 objCount = strtoul(str.c_str(), nullptr, 10u);
        for (U32 i = 0u; i < objCount; ++i)
        {
            std::getline(stream, str);
        }
    }

    return ok;
}

SwBool Scene::loadBinary(std::ifstream& stream)
{
    UNUSED_ARG(stream);
    return false;
}
