
/* Include files */

#include "Scene.hpp"

#include <fstream>
#include <iostream>

#include "Animated.hpp"
#include "AnimatedTextureMesh.hpp"
#include "GlowMesh.hpp"
#include "HeightMap.hpp"
#include "MeshShader.hpp"
#include "ParticleSystem.hpp"
#include "RegularMesh.hpp"
#include "Sky.hpp"

#include <glm/gtc/matrix_transform.hpp>

#include <algorithm>
#include <swizzle/asset/TextureLoader.hpp>
#include <swizzle/asset2/Assets.hpp>

/* Defines */

/* Typedefs */

/* Structs/Classes */

/* Static Variables */

/* Static Function Declaration */

/* Static Function Definition */

/* Function Definition */

/* Class Public Function Definition */

Scene::Scene(common::Resource<swizzle::gfx::GfxContext> ctx, common::Resource<Compositor> compositor,
             common::Resource<AssetManager> assetManager)
    : mContext(ctx)
    , mCompositor(compositor)
    , mAssetManager(assetManager)
    , mSceneState(SceneState::NotLoaded)
{
}

Scene::~Scene() {}

void Scene::clear() {}

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
    common::Resource<swizzle::gfx::Shader> skyShader;
    common::Resource<swizzle::gfx::Texture> skyTexture;
    common::Resource<swizzle::gfx::Material> skyMaterial;

    swizzle::gfx::ShaderAttributeList attribsSky = {};
    attribsSky.mBufferInput = {{swizzle::gfx::ShaderBufferInputRate::InputRate_Vertex, sizeof(float) * (3U + 3U + 2U)}};
    attribsSky.mAttributes = {{0U, swizzle::gfx::ShaderAttributeDataType::vec3f, 0U},
                              {0U, swizzle::gfx::ShaderAttributeDataType::vec3f, sizeof(float) * 3U},
                              {0U, swizzle::gfx::ShaderAttributeDataType::vec2f, sizeof(float) * 6U}};
    attribsSky.mDescriptors = {
        {swizzle::gfx::DescriptorType::TextureSampler,
         swizzle::gfx::Count(1u),
         {swizzle::gfx::StageType::fragmentStage}},
    };
    attribsSky.mPushConstantSize = sizeof(glm::mat4) * 4u;
    attribsSky.mEnableDepthTest = false;
    attribsSky.mEnableBlending = false;
    attribsSky.mEnableDepthWrite = false;
    attribsSky.mPrimitiveType = swizzle::gfx::PrimitiveType::triangle;

    skyShader = mCompositor->createShader(1u, attribsSky);
    mAssetManager->loadShader(skyShader, "shaders/sky.shader");

    skyMaterial = mContext->createMaterial(skyShader, swizzle::gfx::SamplerMode::SamplerModeClamp);

    skyTexture =
        swizzle::asset::LoadTextureCubeMap(mContext, "texture/right.png", "texture/left.png", "texture/top.png",
                                           "texture/bottom.png", "texture/front.png", "texture/back.png");

    skyMaterial->setDescriptorTextureResource(0, skyTexture);

    swizzle::asset2::MeshAssetLoaderDescription ldi = {};
    ldi.mLoadPossitions = {
        {swizzle::asset2::AttributeTypes::VertexPosition, 0u},
        {swizzle::asset2::AttributeTypes::NormalVector, sizeof(float) * 3u},
        {swizzle::asset2::AttributeTypes::UvCoordinates, sizeof(float) * 6u},
    };

    auto mesh2 = swizzle::asset2::LoadMesh("meshes/inverted_sphere.obj", ldi);

    auto vertexBuffer = mContext->createBuffer(swizzle::gfx::BufferType::Vertex);
    vertexBuffer->setBufferData((U8*)mesh2->getVertexDataPtr(), mesh2->getVertexDataSize(),
                                sizeof(float) * (3 + 3 + 2));

    mRenderables.emplace_back(common::CreateRef<Sky>(vertexBuffer, skyTexture, skyMaterial, skyShader));
}

void Scene::loadAnimMesh()
{
    namespace sgfx = swizzle::gfx;

    swizzle::asset2::MeshAssetLoaderDescription ldi = {};
    ldi.mLoadPossitions = {
        {swizzle::asset2::AttributeTypes::VertexPosition, 0u},
        {swizzle::asset2::AttributeTypes::NormalVector, sizeof(float) * 3u},
        {swizzle::asset2::AttributeTypes::UvCoordinates, sizeof(float) * 6u},
        {swizzle::asset2::AttributeTypes::BoneIndices, sizeof(float) * 8u},
        {swizzle::asset2::AttributeTypes::BoneWeights, sizeof(float) * 12u},
    };

    auto mesh2 = swizzle::asset2::LoadMesh("meshes/test.swm", ldi);

    common::Resource<sgfx::Buffer> verts = mContext->createBuffer(sgfx::BufferType::Vertex);
    common::Resource<sgfx::Buffer> idx = mContext->createBuffer(sgfx::BufferType::Index);

    verts->setBufferData((U8*)mesh2->getVertexDataPtr(), mesh2->getVertexDataSize(),
                         sizeof(float) * (3u + 3u + 2u + 4u + 4u));
    idx->setBufferData((U8*)mesh2->getIndexDataPtr(), mesh2->getIndexDataSize(), sizeof(U32) * 3u);

    common::Resource<sgfx::Buffer> instBuffer = mContext->createBuffer(sgfx::BufferType::Vertex);

    std::vector<glm::mat4> positions;

    for (int i = -4; i < 4; ++i)
    {
        glm::mat4 m = glm::mat4(1.0f);
        m = glm::translate(m, {i, 0, 0});
        m = glm::rotate(m, 90.0f, glm::vec3(0, 1.0, 0));
        positions.push_back(m);
    }

    instBuffer->setBufferData(positions.data(), sizeof(glm::mat4) * positions.size(), sizeof(glm::mat4));

    common::Resource<sgfx::Shader> shader;
    common::Resource<sgfx::Texture> texture;

    sgfx::ShaderAttributeList attribsAnim = {};
    attribsAnim.mBufferInput = {
        {sgfx::ShaderBufferInputRate::InputRate_Vertex, sizeof(float) * (3u + 3u + 2u + 4u + 4u)},
        {sgfx::ShaderBufferInputRate::InputRate_Instance, sizeof(float) * (16u)}};
    attribsAnim.mAttributes = {
        {0u, sgfx::ShaderAttributeDataType::vec3f, 0u},
        {0u, sgfx::ShaderAttributeDataType::vec3f, sizeof(float) * 3u},
        {0u, sgfx::ShaderAttributeDataType::vec2f, sizeof(float) * 6u},
        {0u, sgfx::ShaderAttributeDataType::vec4u, sizeof(float) * 8u},
        {0u, sgfx::ShaderAttributeDataType::vec4f, sizeof(float) * 12u},
        {1u, sgfx::ShaderAttributeDataType::vec4f, 0u},
        {1u, sgfx::ShaderAttributeDataType::vec4f, sizeof(float) * 4u},
        {1u, sgfx::ShaderAttributeDataType::vec4f, sizeof(float) * 8u},
        {1u, sgfx::ShaderAttributeDataType::vec4f, sizeof(float) * 12u},
    };
    attribsAnim.mDescriptors = {
        {sgfx::DescriptorType::TextureSampler, sgfx::Count(1u), {sgfx::StageType::fragmentStage}},
        {sgfx::DescriptorType::UniformBuffer,
         sgfx::Count(1u),
         {sgfx::StageType::fragmentStage, sgfx::StageType::vertexStage}},
    };
    attribsAnim.mPushConstantSize = sizeof(glm::mat4) * 2u;
    attribsAnim.mEnableDepthTest = true;
    attribsAnim.mEnableBlending = false;
    attribsAnim.mEnableDepthWrite = true;
    attribsAnim.mPrimitiveType = swizzle::gfx::PrimitiveType::triangle;

    texture = swizzle::asset::LoadTexture2D(mContext, "texture/lightGray.png");

    shader = mCompositor->createShader(1u, attribsAnim);
    mAssetManager->loadShader(shader, "shaders/animated_inst.shader");

    mRenderables.emplace_back(common::CreateRef<Animated>(mContext, mesh2, instBuffer, texture, shader));
}

void Scene::loadCube()
{
    namespace sgfx = swizzle::gfx;

    swizzle::asset2::MeshAssetLoaderDescription ldi = {};
    ldi.mLoadPossitions = {
        {swizzle::asset2::AttributeTypes::VertexPosition, 0u},
        {swizzle::asset2::AttributeTypes::NormalVector, sizeof(float) * 3u},
        {swizzle::asset2::AttributeTypes::UvCoordinates, sizeof(float) * 6u},
    };

    auto mesh2 = swizzle::asset2::LoadMesh("AoG/meshes/cube.swm", ldi);

    common::Resource<sgfx::Buffer> instBuffer = mContext->createBuffer(sgfx::BufferType::Vertex);

    std::vector<glm::mat4> positions;

    glm::mat4 m = glm::mat4(1.0f);
    m = glm::translate(m, {0, 0, -5});
    positions.push_back(m);

    instBuffer->setBufferData(positions.data(), sizeof(glm::mat4) * positions.size(), sizeof(glm::mat4));

    swizzle::gfx::ShaderAttributeList attribs = {};
    attribs.mBufferInput = {{swizzle::gfx::ShaderBufferInputRate::InputRate_Vertex, sizeof(float) * (3u + 3u + 2u)},
                            {sgfx::ShaderBufferInputRate::InputRate_Instance, sizeof(float) * (16u)}};
    attribs.mAttributes = {
        {0u, swizzle::gfx::ShaderAttributeDataType::vec3f, 0u},
        {0u, swizzle::gfx::ShaderAttributeDataType::vec3f, sizeof(float) * 3u},
        {0u, swizzle::gfx::ShaderAttributeDataType::vec2f, sizeof(float) * 6u},
        {1u, sgfx::ShaderAttributeDataType::vec4f, 0u},
        {1u, sgfx::ShaderAttributeDataType::vec4f, sizeof(float) * 4u},
        {1u, sgfx::ShaderAttributeDataType::vec4f, sizeof(float) * 8u},
        {1u, sgfx::ShaderAttributeDataType::vec4f, sizeof(float) * 12u},
    };
    attribs.mDescriptors = {
        {swizzle::gfx::DescriptorType::TextureSampler,
         swizzle::gfx::Count(1u),
         {swizzle::gfx::StageType::fragmentStage}},
    };
    attribs.mPushConstantSize = sizeof(glm::mat4) * 2u;
    attribs.mEnableDepthTest = true;
    attribs.mEnableBlending = false;
    attribs.mEnableDepthWrite = true;
    attribs.mPrimitiveType = swizzle::gfx::PrimitiveType::triangle;

    auto shader = mCompositor->createShader(1u, attribs);
    mAssetManager->loadShader(shader, "AoG/shaders/regular.shader");

    auto texture = swizzle::asset::LoadTexture2D(mContext, "AoG/textures/cubeNormal.png");
    auto optTexture = swizzle::asset::LoadTexture2D(mContext, "AoG/textures/neutral.png");

    mRenderables.emplace_back(
        common::CreateRef<RegularMesh>(mContext, mesh2, instBuffer, texture, optTexture, shader, shader));
}

void Scene::loadAnimTexture()
{
    glm::mat4 transform = glm::mat4(1.0f);
    transform = glm::translate(transform, {-4, 0, 0});

    namespace sgfx = swizzle::gfx;

    swizzle::asset2::MeshAssetLoaderDescription ldi = {};
    ldi.mLoadPossitions = {
        {swizzle::asset2::AttributeTypes::VertexPosition, 0u},
        {swizzle::asset2::AttributeTypes::NormalVector, sizeof(float) * 3u},
        {swizzle::asset2::AttributeTypes::UvCoordinates, sizeof(float) * 6u},
    };

    auto mesh2 = swizzle::asset2::LoadMesh("AoG/meshes/TextureAnimation.swm", ldi);

    common::Resource<sgfx::Buffer> instBuffer = mContext->createBuffer(sgfx::BufferType::Vertex);

    std::vector<glm::mat4> positions;
    positions.push_back(transform);

    instBuffer->setBufferData(positions.data(), sizeof(glm::mat4) * positions.size(), sizeof(glm::mat4));

    swizzle::gfx::ShaderAttributeList attribs = {};
    attribs.mBufferInput = {{swizzle::gfx::ShaderBufferInputRate::InputRate_Vertex, sizeof(float) * (3u + 3u + 2u)},
                            {sgfx::ShaderBufferInputRate::InputRate_Instance, sizeof(float) * (16u)}};
    attribs.mAttributes = {
        {0u, swizzle::gfx::ShaderAttributeDataType::vec3f, 0u},
        {0u, swizzle::gfx::ShaderAttributeDataType::vec3f, sizeof(float) * 3u},
        {0u, swizzle::gfx::ShaderAttributeDataType::vec2f, sizeof(float) * 6u},
        {1u, sgfx::ShaderAttributeDataType::vec4f, 0u},
        {1u, sgfx::ShaderAttributeDataType::vec4f, sizeof(float) * 4u},
        {1u, sgfx::ShaderAttributeDataType::vec4f, sizeof(float) * 8u},
        {1u, sgfx::ShaderAttributeDataType::vec4f, sizeof(float) * 12u},
    };
    attribs.mDescriptors = {
        {swizzle::gfx::DescriptorType::TextureSampler,
         swizzle::gfx::Count(1u),
         {swizzle::gfx::StageType::fragmentStage}},
    };
    attribs.mPushConstantSize = sizeof(glm::mat4) * 2u;
    attribs.mEnableDepthTest = true;
    attribs.mEnableBlending = false;
    attribs.mEnableDepthWrite = true;
    attribs.mPrimitiveType = swizzle::gfx::PrimitiveType::triangle;

    auto shader = mCompositor->createShader(1u, attribs);
    mAssetManager->loadShader(shader, "AoG/shaders/animatedTexture.shader");

    auto texture = swizzle::asset::LoadTexture2D(mContext, "AoG/textures/Stream.png");

    mRenderables.emplace_back(common::CreateRef<AnimatedTextureMesh>(mContext, mesh2, instBuffer, texture, shader));
}

void Scene::loadHeightMap()
{
    glm::mat4 transform = glm::mat4(1.0f);
    transform = glm::translate(transform, {0, 0, 0});

    namespace sgfx = swizzle::gfx;

    common::Resource<sgfx::Buffer> instBuffer = mContext->createBuffer(sgfx::BufferType::Vertex);

    std::vector<glm::mat4> positions;
    positions.push_back(transform);

    instBuffer->setBufferData(positions.data(), sizeof(glm::mat4) * positions.size(), sizeof(glm::mat4));

    swizzle::gfx::ShaderAttributeList attribs = {};
    attribs.mBufferInput = {{swizzle::gfx::ShaderBufferInputRate::InputRate_Vertex, sizeof(float) * (3u + 3u + 2u)},
                            {sgfx::ShaderBufferInputRate::InputRate_Instance, sizeof(float) * (16u)}};
    attribs.mAttributes = {
        {0u, swizzle::gfx::ShaderAttributeDataType::vec3f, 0u},
        {0u, swizzle::gfx::ShaderAttributeDataType::vec3f, sizeof(float) * 3u},
        {0u, swizzle::gfx::ShaderAttributeDataType::vec2f, sizeof(float) * 6u},
        {1u, sgfx::ShaderAttributeDataType::vec4f, 0u},
        {1u, sgfx::ShaderAttributeDataType::vec4f, sizeof(float) * 4u},
        {1u, sgfx::ShaderAttributeDataType::vec4f, sizeof(float) * 8u},
        {1u, sgfx::ShaderAttributeDataType::vec4f, sizeof(float) * 12u},
    };
    attribs.mDescriptors = {
        {swizzle::gfx::DescriptorType::TextureSampler,
         swizzle::gfx::Count(1u),
         {swizzle::gfx::StageType::fragmentStage}},
    };
    attribs.mPushConstantSize = sizeof(glm::mat4) * 2u;
    attribs.mEnableDepthTest = true;
    attribs.mEnableBlending = false;
    attribs.mEnableDepthWrite = true;
    attribs.mPrimitiveType = swizzle::gfx::PrimitiveType::triangle;

    auto shader = mCompositor->createShader(1u, attribs);
    mAssetManager->loadShader(shader, "AoG/shaders/heightMap.shader");

    auto texture = swizzle::asset::LoadTexture2D(mContext, "AoG/textures/heightmap.png");

    mRenderables.emplace_back(common::CreateRef<HeightMap>(mContext, instBuffer, texture, shader));
}

void Scene::loadTesselationMesh()
{
    namespace sgfx = swizzle::gfx;

    swizzle::asset2::MeshAssetLoaderDescription ldi = {};
    ldi.mLoadPossitions = {
        {swizzle::asset2::AttributeTypes::VertexPosition, 0u},
        {swizzle::asset2::AttributeTypes::NormalVector, sizeof(float) * 3u},
        {swizzle::asset2::AttributeTypes::UvCoordinates, sizeof(float) * 6u},
    };

    auto mesh2 = swizzle::asset2::LoadMesh("AoG/meshes/elevatedPlane.swm", ldi);

    common::Resource<sgfx::Buffer> instBuffer = mContext->createBuffer(sgfx::BufferType::Vertex);

    std::vector<glm::mat4> positions;

    glm::mat4 m = glm::mat4(1.0f);
    m = glm::translate(m, {0, 0, 0});
    positions.push_back(m);

    instBuffer->setBufferData(positions.data(), sizeof(glm::mat4) * positions.size(), sizeof(glm::mat4));

    swizzle::gfx::ShaderAttributeList attribs = {};
    attribs.mBufferInput = {{swizzle::gfx::ShaderBufferInputRate::InputRate_Vertex, sizeof(float) * (3u + 3u + 2u)},
                            {sgfx::ShaderBufferInputRate::InputRate_Instance, sizeof(float) * (16u)}};
    attribs.mAttributes = {
        {0u, swizzle::gfx::ShaderAttributeDataType::vec3f, 0u},
        {0u, swizzle::gfx::ShaderAttributeDataType::vec3f, sizeof(float) * 3u},
        {0u, swizzle::gfx::ShaderAttributeDataType::vec2f, sizeof(float) * 6u},
        {1u, sgfx::ShaderAttributeDataType::vec4f, 0u},
        {1u, sgfx::ShaderAttributeDataType::vec4f, sizeof(float) * 4u},
        {1u, sgfx::ShaderAttributeDataType::vec4f, sizeof(float) * 8u},
        {1u, sgfx::ShaderAttributeDataType::vec4f, sizeof(float) * 12u},
    };
    attribs.mDescriptors = {
        {swizzle::gfx::DescriptorType::TextureSampler,
         swizzle::gfx::Count(1u),
         {swizzle::gfx::StageType::fragmentStage, swizzle::gfx::StageType::tessellationStage}},
    };
    attribs.mPushConstantSize = sizeof(glm::mat4) * 2u;
    attribs.mEnableDepthTest = true;
    attribs.mEnableBlending = false;
    attribs.mEnableDepthWrite = true;
    attribs.mPrimitiveType = swizzle::gfx::PrimitiveType::triangle;

    auto shader = mCompositor->createShader(1u, attribs);
    mAssetManager->loadShader(shader, "AoG/shaders/tesselationShader.shader");

    auto otherShader = mCompositor->createShader(1u, attribs);
    mAssetManager->loadShader(otherShader, "AoG/shaders/heightMap.shader");

    auto texture = swizzle::asset::LoadTexture2D(mContext, "AoG/textures/displaceTest.png");

    mRenderables.emplace_back(
        common::CreateRef<RegularMesh>(mContext, mesh2, instBuffer, texture, texture, shader, otherShader));
}

void Scene::loadParticleSystem()
{
    namespace sgfx = swizzle::gfx;

    sgfx::ShaderAttributeList attribs = {};
    attribs.mBufferInput = {{sgfx::ShaderBufferInputRate::InputRate_Vertex, sizeof(float) * 3u}};
    attribs.mAttributes = {{0u, sgfx::ShaderAttributeDataType::vec3f, 0u}};
    attribs.mDescriptors = {
        {sgfx::DescriptorType::TextureSampler, sgfx::Count(1u), {sgfx::StageType::fragmentStage}},
    };
    attribs.mPushConstantSize = sizeof(glm::mat4) * 2u;
    attribs.mEnableDepthTest = true;
    attribs.mEnableBlending = false;
    attribs.mEnableDepthWrite = true;
    attribs.mPrimitiveType = swizzle::gfx::PrimitiveType::triangle;

    auto shader = mCompositor->createShader(1u, attribs);
    mAssetManager->loadShader(shader, "AoG/shaders/particle.shader");

    auto texture = swizzle::asset::LoadTexture2D(mContext, "AoG/textures/particle.png");

    mRenderables.emplace_back(
        common::CreateRef<ParticleSystem>(mContext, 200u, glm::vec3{0, 5, 0}, glm::vec3{0, 1, 0}, texture, shader));
}

void Scene::loadGlow()
{
    glm::mat4 transform = glm::mat4(1.0f);
    transform = glm::translate(transform, {4, 0, 0});

    namespace sgfx = swizzle::gfx;

    swizzle::asset2::MeshAssetLoaderDescription ldi = {};
    ldi.mLoadPossitions = {
        {swizzle::asset2::AttributeTypes::VertexPosition, 0u},
        {swizzle::asset2::AttributeTypes::NormalVector, sizeof(float) * 3u},
        {swizzle::asset2::AttributeTypes::UvCoordinates, sizeof(float) * 6u},
    };

    auto mesh2 = swizzle::asset2::LoadMesh("AoG/meshes/TextureAnimation.swm", ldi);

    common::Resource<sgfx::Buffer> instBuffer = mContext->createBuffer(sgfx::BufferType::Vertex);

    std::vector<glm::mat4> positions;
    positions.push_back(transform);

    instBuffer->setBufferData(positions.data(), sizeof(glm::mat4) * positions.size(), sizeof(glm::mat4));

    swizzle::gfx::ShaderAttributeList attribs = {};
    attribs.mBufferInput = {{swizzle::gfx::ShaderBufferInputRate::InputRate_Vertex, sizeof(float) * (3u + 3u + 2u)},
                            {sgfx::ShaderBufferInputRate::InputRate_Instance, sizeof(float) * (16u)}};
    attribs.mAttributes = {
        {0u, swizzle::gfx::ShaderAttributeDataType::vec3f, 0u},
        {0u, swizzle::gfx::ShaderAttributeDataType::vec3f, sizeof(float) * 3u},
        {0u, swizzle::gfx::ShaderAttributeDataType::vec2f, sizeof(float) * 6u},
        {1u, sgfx::ShaderAttributeDataType::vec4f, 0u},
        {1u, sgfx::ShaderAttributeDataType::vec4f, sizeof(float) * 4u},
        {1u, sgfx::ShaderAttributeDataType::vec4f, sizeof(float) * 8u},
        {1u, sgfx::ShaderAttributeDataType::vec4f, sizeof(float) * 12u},
    };
    attribs.mDescriptors = {{swizzle::gfx::DescriptorType::TextureSampler,
                             swizzle::gfx::Count(1u),
                             {swizzle::gfx::StageType::fragmentStage}},
                            {swizzle::gfx::DescriptorType::TextureSampler,
                             swizzle::gfx::Count(1u),
                             {swizzle::gfx::StageType::fragmentStage}}};
    attribs.mPushConstantSize = sizeof(glm::mat4) * 2u;
    attribs.mEnableDepthTest = true;
    attribs.mEnableBlending = false;
    attribs.mEnableDepthWrite = true;
    attribs.mPrimitiveType = swizzle::gfx::PrimitiveType::triangle;

    auto shader = mCompositor->createShader(1u, attribs);
    mAssetManager->loadShader(shader, "AoG/shaders/glow.shader");

    auto grid = swizzle::asset::LoadTexture2D(mContext, "AoG/textures/Grid.png");
    auto glow = swizzle::asset::LoadTexture2D(mContext, "AoG/textures/Glow.png");

    mRenderables.emplace_back(common::CreateRef<GlowMesh>(mContext, mesh2, instBuffer, grid, glow, shader));
}

void Scene::loadMeshShader()
{
    glm::mat4 transform = glm::mat4(1.0f);
    transform = glm::translate(transform, {0, 0, 0});

    namespace sgfx = swizzle::gfx;

    swizzle::asset2::MeshAssetLoaderDescription ldi = {};
    ldi.mLoadPossitions = {
        {swizzle::asset2::AttributeTypes::VertexPosition, 0u},
        //{swizzle::asset2::AttributeTypes::NormalVector, sizeof(float) * 3u},
    };

    auto mesh2 = swizzle::asset2::LoadMesh("AoG/meshes/torus.swm", ldi);

    common::Resource<sgfx::Buffer> instBuffer = mContext->createBuffer(sgfx::BufferType::Vertex);

    std::vector<glm::mat4> positions;
    positions.push_back(transform);

    instBuffer->setBufferData(positions.data(), sizeof(glm::mat4) * positions.size(), sizeof(glm::mat4));

    swizzle::gfx::ShaderAttributeList attribs = {};
    attribs.mBufferInput = {};
    attribs.mAttributes = {};
    attribs.mDescriptors = {
        {sgfx::DescriptorType::StorageBuffer, 1u, {sgfx::StageType::meshStage}},
        {sgfx::DescriptorType::StorageBuffer, 1u, {sgfx::StageType::meshStage}},
        {sgfx::DescriptorType::StorageBuffer, 1u, {sgfx::StageType::meshStage}},
        {sgfx::DescriptorType::StorageBuffer, 1u, {sgfx::StageType::meshStage}},
    };
    attribs.mPushConstantSize = sizeof(glm::mat4) * 2u;
    attribs.mEnableDepthTest = true;
    attribs.mEnableBlending = false;
    attribs.mEnableDepthWrite = true;
    attribs.mPrimitiveType = swizzle::gfx::PrimitiveType::triangle;

    auto shader = mCompositor->createMeshShader(1u, attribs);
    if (mContext->hasMeshShaderSupport())
    {
        mAssetManager->loadShader(shader, "AoG/shaders/meshShader.shader");
    }

    swizzle::gfx::ShaderAttributeList attribs2 = {};
    attribs2.mBufferInput = {{swizzle::gfx::ShaderBufferInputRate::InputRate_Vertex, sizeof(float) * (3u)},
                             {sgfx::ShaderBufferInputRate::InputRate_Instance, sizeof(float) * (16u)}};
    attribs2.mAttributes = {
        {0u, swizzle::gfx::ShaderAttributeDataType::vec3f, 0u},
        {1u, sgfx::ShaderAttributeDataType::vec4f, 0u},
        {1u, sgfx::ShaderAttributeDataType::vec4f, sizeof(float) * 4u},
        {1u, sgfx::ShaderAttributeDataType::vec4f, sizeof(float) * 8u},
        {1u, sgfx::ShaderAttributeDataType::vec4f, sizeof(float) * 12u},
    };
    attribs2.mDescriptors = {};
    attribs2.mPushConstantSize = sizeof(glm::mat4) * 2u;
    attribs2.mEnableDepthTest = true;
    attribs2.mEnableBlending = false;
    attribs2.mEnableDepthWrite = true;
    attribs2.mPrimitiveType = swizzle::gfx::PrimitiveType::triangle;

    auto shaderNormal = mCompositor->createShader(1u, attribs2);
    mAssetManager->loadShader(shaderNormal, "AoG/shaders/normal.shader");

    mRenderables.emplace_back(common::CreateRef<MeshShader>(mContext, mesh2, instBuffer, shader, shaderNormal));
}

SceneState Scene::update(DeltaTime dt, SceneRenderSettings& settings,
                         common::Unique<swizzle::gfx::CommandTransaction>& trans)
{
    for (auto& it : mRenderables)
    {
        it->update(dt, settings, trans);
    }

    return mSceneState;
}

void Scene::render(common::Unique<swizzle::gfx::DrawCommandTransaction>& trans, PerspectiveCamera& cam)
{
    for (auto& it : mRenderables)
    {
        it->render(trans, cam);
    }
}

/* Class Protected Function Definition */

/* Class Private Function Definition */

SwBool Scene::loadAscii(std::ifstream& stream)
{
    UNUSED_ARG(stream);
    SwBool ok = true;

    return ok;
}