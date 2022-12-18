
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
#include <swizzle/asset2/Assets.hpp>

/* Defines */

/* Typedefs */

namespace sgfx = swizzle::gfx;

/* Structs/Classes */

/* Static Variables */

/* Static Function Declaration */

/* Static Function Definition */

std::vector<std::string> split(std::string s, std::string delimiter)
{
    size_t pos_start = 0, pos_end, delim_len = delimiter.length();
    std::string token;
    std::vector<std::string> res;

    while ((pos_end = s.find(delimiter, pos_start)) != std::string::npos)
    {
        token = s.substr(pos_start, pos_end - pos_start);
        pos_start = pos_end + delim_len;
        res.push_back(token);
    }

    res.push_back(s.substr(pos_start));
    return res;
}

glm::vec3 vec3FromString(std::string& str)
{
    str.pop_back();
    str.erase(0, 1);

    std::vector<std::string> values = split(str, ",");

    return {std::stof(values[0]), std::stof(values[1]), std::stof(values[2])};
}

glm::mat4 mat4FromStringTransform(std::string& str)
{
    glm::vec3 tfm = vec3FromString(str);
    glm::mat4 transform = glm::mat4(1.0f);
    transform = glm::translate(transform, tfm);

    return transform;
}

std::vector<glm::mat4> instancesFromString(std::string& str)
{
    str.pop_back();
    str.erase(0, 1);

    std::vector<std::string> values = split(str, ":");

    std::vector<glm::mat4> mats;

    for (auto& s : values)
    {
        mats.push_back(mat4FromStringTransform(s));
    }

    return mats;
}

/* Function Definition */

/* Class Public Function Definition */

Scene::Scene(common::Resource<sgfx::GfxContext> ctx, common::Resource<Compositor> compositor,
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
    common::Resource<sgfx::Shader> skyShader;
    // common::Resource<sgfx::Texture> skyTexture;
    common::Resource<sgfx::Material> skyMaterial;

    sgfx::ShaderAttributeList attribsSky = {};
    attribsSky.mBufferInput = {{sgfx::ShaderBufferInputRate::InputRate_Vertex, sizeof(float) * (3U + 3U + 2U)},
                               {sgfx::ShaderBufferInputRate::InputRate_Instance, sizeof(float) * (16u)}};
    attribsSky.mAttributes = {
        {0U, sgfx::ShaderAttributeDataType::vec3f, 0U},
        {0U, sgfx::ShaderAttributeDataType::vec3f, sizeof(float) * 3U},
        {0U, sgfx::ShaderAttributeDataType::vec2f, sizeof(float) * 6U},
        {1u, sgfx::ShaderAttributeDataType::vec4f, 0u},
        {1u, sgfx::ShaderAttributeDataType::vec4f, sizeof(float) * 4u},
        {1u, sgfx::ShaderAttributeDataType::vec4f, sizeof(float) * 8u},
        {1u, sgfx::ShaderAttributeDataType::vec4f, sizeof(float) * 12u},
    };
    attribsSky.mDescriptors = {
        {sgfx::DescriptorType::UniformBuffer,
         sgfx::Count(1u),
         {sgfx::StageType::vertexStage, sgfx::StageType::fragmentStage}},
    };
    attribsSky.mPushConstantSize = sizeof(glm::mat4) * 4u;
    attribsSky.mEnableDepthTest = false;
    attribsSky.mEnableBlending = false;
    attribsSky.mEnableDepthWrite = false;
    attribsSky.mPrimitiveType = swizzle::gfx::PrimitiveType::triangle;

    skyShader = mCompositor->createShader(1u, attribsSky);
    mAssetManager->loadShader(skyShader, "AoG/shaders/sky.shader");

    skyMaterial = mContext->createMaterial(skyShader, sgfx::SamplerMode::SamplerModeClamp);

    swizzle::asset2::MeshAssetLoaderDescription ldi = {};
    ldi.mLoadPossitions = {
        {swizzle::asset2::AttributeTypes::VertexPosition, 0u},
        {swizzle::asset2::AttributeTypes::NormalVector, sizeof(float) * 3u},
        {swizzle::asset2::AttributeTypes::UvCoordinates, sizeof(float) * 6u},
    };

    auto mesh2 = swizzle::asset2::LoadMesh("AoG/meshes/inverted_sphere.obj", ldi);

    auto vertexBuffer = mContext->createBuffer(sgfx::BufferType::Vertex);
    vertexBuffer->setBufferData((U8*)mesh2->getVertexDataPtr(), mesh2->getVertexDataSize(),
                                sizeof(float) * (3 + 3 + 2));

    auto uniformBuffer = mContext->createBuffer(sgfx::BufferType::UniformBuffer);
    auto inst = mContext->createBuffer(sgfx::BufferType::Vertex);

    mRenderables.emplace_back(common::CreateRef<Sky>(vertexBuffer, skyMaterial, skyShader, uniformBuffer, inst));
}

void Scene::loadAnimMesh()
{
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

    texture = mAssetManager->loadTexture("texture/lightGray.png");

    shader = mCompositor->createShader(1u, attribsAnim);
    mAssetManager->loadShader(shader, "shaders/animated_inst.shader");

    mRenderables.emplace_back(common::CreateRef<Animated>(mContext, mesh2, instBuffer, texture, shader));
}

void Scene::loadHeightMap()
{
    glm::mat4 transform = glm::mat4(1.0f);
    transform = glm::translate(transform, {0, 0, 0});

    common::Resource<sgfx::Buffer> instBuffer = mContext->createBuffer(sgfx::BufferType::Vertex);

    std::vector<glm::mat4> positions;
    positions.push_back(transform);

    instBuffer->setBufferData(positions.data(), sizeof(glm::mat4) * positions.size(), sizeof(glm::mat4));

    sgfx::ShaderAttributeList attribs = {};
    attribs.mBufferInput = {{sgfx::ShaderBufferInputRate::InputRate_Vertex, sizeof(float) * (3u + 3u + 2u)},
                            {sgfx::ShaderBufferInputRate::InputRate_Instance, sizeof(float) * (16u)}};
    attribs.mAttributes = {
        {0u, sgfx::ShaderAttributeDataType::vec3f, 0u},
        {0u, sgfx::ShaderAttributeDataType::vec3f, sizeof(float) * 3u},
        {0u, sgfx::ShaderAttributeDataType::vec2f, sizeof(float) * 6u},
        {1u, sgfx::ShaderAttributeDataType::vec4f, 0u},
        {1u, sgfx::ShaderAttributeDataType::vec4f, sizeof(float) * 4u},
        {1u, sgfx::ShaderAttributeDataType::vec4f, sizeof(float) * 8u},
        {1u, sgfx::ShaderAttributeDataType::vec4f, sizeof(float) * 12u},
    };
    attribs.mDescriptors = {
        {sgfx::DescriptorType::UniformBuffer, sgfx::Count(1u), {sgfx::StageType::vertexStage}},
        {sgfx::DescriptorType::TextureSampler, sgfx::Count(1u), {sgfx::StageType::fragmentStage}},
    };
    attribs.mPushConstantSize = sizeof(glm::mat4) * 2u;
    attribs.mEnableDepthTest = true;
    attribs.mEnableBlending = false;
    attribs.mEnableDepthWrite = true;
    attribs.mPrimitiveType = swizzle::gfx::PrimitiveType::triangle;

    auto shader = mCompositor->createShader(1u, attribs);
    mAssetManager->loadShader(shader, "AoG/shaders/heightMap.shader");

    auto texture = mAssetManager->loadTexture("AoG/textures/heightmap.png");

    mRenderables.emplace_back(common::CreateRef<HeightMap>(mContext, instBuffer, texture, shader));
}

void Scene::loadTesselationMesh()
{
    auto asset = mAssetManager->loadMesh("AoG/meshes/elevatedPlane.swm", false);

    common::Resource<sgfx::Buffer> instBuffer = mContext->createBuffer(sgfx::BufferType::Vertex);

    std::vector<glm::mat4> positions;

    glm::mat4 m = glm::mat4(1.0f);
    m = glm::translate(m, {0, 0, 0});
    positions.push_back(m);

    instBuffer->setBufferData(positions.data(), sizeof(glm::mat4) * positions.size(), sizeof(glm::mat4));

    sgfx::ShaderAttributeList attribs = {};
    attribs.mBufferInput = {{sgfx::ShaderBufferInputRate::InputRate_Vertex, sizeof(float) * (3u + 3u + 2u)},
                            {sgfx::ShaderBufferInputRate::InputRate_Instance, sizeof(float) * (16u)}};
    attribs.mAttributes = {
        {0u, sgfx::ShaderAttributeDataType::vec3f, 0u},
        {0u, sgfx::ShaderAttributeDataType::vec3f, sizeof(float) * 3u},
        {0u, sgfx::ShaderAttributeDataType::vec2f, sizeof(float) * 6u},
        {1u, sgfx::ShaderAttributeDataType::vec4f, 0u},
        {1u, sgfx::ShaderAttributeDataType::vec4f, sizeof(float) * 4u},
        {1u, sgfx::ShaderAttributeDataType::vec4f, sizeof(float) * 8u},
        {1u, sgfx::ShaderAttributeDataType::vec4f, sizeof(float) * 12u},
    };
    attribs.mDescriptors = {
        {sgfx::DescriptorType::UniformBuffer, sgfx::Count(1u), {sgfx::StageType::vertexStage}},
        {sgfx::DescriptorType::TextureSampler,
         sgfx::Count(1u),
         {sgfx::StageType::fragmentStage, sgfx::StageType::tessellationStage}},
        {sgfx::DescriptorType::TextureSampler,
         sgfx::Count(1u),
         {sgfx::StageType::fragmentStage, sgfx::StageType::tessellationStage}},
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

    auto texture = mAssetManager->loadTexture("AoG/textures/displaceTest.png");

    mRenderables.emplace_back(
        common::CreateRef<RegularMesh>(mContext, asset, instBuffer, texture, texture, texture, shader, otherShader));
}

void Scene::loadGlow()
{
    glm::mat4 transform = glm::mat4(1.0f);
    transform = glm::translate(transform, {4, 0, 0});

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

    sgfx::ShaderAttributeList attribs = {};
    attribs.mBufferInput = {{sgfx::ShaderBufferInputRate::InputRate_Vertex, sizeof(float) * (3u + 3u + 2u)},
                            {sgfx::ShaderBufferInputRate::InputRate_Instance, sizeof(float) * (16u)}};
    attribs.mAttributes = {
        {0u, sgfx::ShaderAttributeDataType::vec3f, 0u},
        {0u, sgfx::ShaderAttributeDataType::vec3f, sizeof(float) * 3u},
        {0u, sgfx::ShaderAttributeDataType::vec2f, sizeof(float) * 6u},
        {1u, sgfx::ShaderAttributeDataType::vec4f, 0u},
        {1u, sgfx::ShaderAttributeDataType::vec4f, sizeof(float) * 4u},
        {1u, sgfx::ShaderAttributeDataType::vec4f, sizeof(float) * 8u},
        {1u, sgfx::ShaderAttributeDataType::vec4f, sizeof(float) * 12u},
    };
    attribs.mDescriptors = {{sgfx::DescriptorType::TextureSampler, sgfx::Count(1u), {sgfx::StageType::fragmentStage}},
                            {sgfx::DescriptorType::TextureSampler, sgfx::Count(1u), {sgfx::StageType::fragmentStage}}};
    attribs.mPushConstantSize = sizeof(glm::mat4) * 2u;
    attribs.mEnableDepthTest = true;
    attribs.mEnableBlending = false;
    attribs.mEnableDepthWrite = true;
    attribs.mPrimitiveType = swizzle::gfx::PrimitiveType::triangle;

    auto shader = mCompositor->createShader(1u, attribs);
    mAssetManager->loadShader(shader, "AoG/shaders/glow.shader");

    auto grid = mAssetManager->loadTexture("AoG/textures/Grid.png");
    auto glow = mAssetManager->loadTexture("AoG/textures/Glow.png");

    mRenderables.emplace_back(common::CreateRef<GlowMesh>(mContext, mesh2, instBuffer, grid, glow, shader));
}

void Scene::loadMeshShader()
{
    glm::mat4 transform = glm::mat4(1.0f);
    transform = glm::translate(transform, {0, 0, 0});

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

    sgfx::ShaderAttributeList attribs = {};
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

    sgfx::ShaderAttributeList attribs2 = {};
    attribs2.mBufferInput = {{sgfx::ShaderBufferInputRate::InputRate_Vertex, sizeof(float) * (3u)},
                             {sgfx::ShaderBufferInputRate::InputRate_Instance, sizeof(float) * (16u)}};
    attribs2.mAttributes = {
        {0u, sgfx::ShaderAttributeDataType::vec3f, 0u},
        {1u, sgfx::ShaderAttributeDataType::vec4f, 0u},
        {1u, sgfx::ShaderAttributeDataType::vec4f, sizeof(float) * 4u},
        {1u, sgfx::ShaderAttributeDataType::vec4f, sizeof(float) * 8u},
        {1u, sgfx::ShaderAttributeDataType::vec4f, sizeof(float) * 12u},
    };
    attribs2.mDescriptors = {
        {sgfx::DescriptorType::UniformBuffer, sgfx::Count(1u), {sgfx::StageType::vertexStage}},
    };
    attribs2.mPushConstantSize = sizeof(glm::mat4) * 2u;
    attribs2.mEnableDepthTest = true;
    attribs2.mEnableBlending = false;
    attribs2.mEnableDepthWrite = true;
    attribs2.mPrimitiveType = swizzle::gfx::PrimitiveType::triangle;

    auto shaderNormal = mCompositor->createShader(1u, attribs2);
    mAssetManager->loadShader(shaderNormal, "AoG/shaders/normal.shader");

    mRenderables.emplace_back(common::CreateRef<MeshShader>(mContext, mesh2, instBuffer, shader, shaderNormal));
}

SceneState Scene::update(DeltaTime dt, SceneRenderSettings& settings, common::Unique<sgfx::CommandTransaction>& trans)
{
    if (!mItemsToLoad.empty())
    {
        if (settings.mAssetSlowLoad)
        {
            if (mSlowTime > 10.0f)
            {
                auto item = mItemsToLoad.back();
                mItemsToLoad.pop_back();
                loadThing(item);
                mSlowTime = 0.0f;
            }
            mSlowTime += dt;
        }
        else
        {
            auto item = mItemsToLoad.back();
            mItemsToLoad.pop_back();
            loadThing(item);
        }
    }

    for (auto& it : mRenderables)
    {
        it->update(dt, settings, trans);
    }

    return mSceneState;
}

void Scene::render(common::Unique<sgfx::DrawCommandTransaction>& trans, PerspectiveCamera& cam)
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

    while (!stream.eof())
    {
        std::string line;
        std::getline(stream, line);

        if (line._Starts_with("#"))
        {
            continue;
        }
        else
        {
            mItemsToLoad.push_back(line);
        }
    }

    return ok;
}

common::Resource<sgfx::Buffer> Scene::createInstanceBuffer(std::vector<glm::mat4>& matrices)
{
    common::Resource<sgfx::Buffer> instBuffer = mContext->createBuffer(sgfx::BufferType::Vertex);
    instBuffer->setBufferData(matrices.data(), sizeof(glm::mat4) * matrices.size(), sizeof(glm::mat4));

    return instBuffer;
}

void Scene::loadThing(std::string line)
{
    if (line._Starts_with("particle"))
    {
        std::vector<std::string> itms = split(line, ";");
        std::string filePath = itms[1];
        std::string pos = itms[2];
        std::string dir = itms[3];
        std::string count = itms[4];

        loadParticleSystemParams(filePath.c_str(), vec3FromString(pos), vec3FromString(dir), std::stoi(count));
    }
    else if (line._Starts_with("mesh"))
    {
        std::vector<std::string> itms = split(line, ";");
        std::string filePath = itms[1];
        std::string texturePath = itms[2];
        std::string normalTexturePath = itms[3];
        std::string instances = itms[4];

        std::vector<glm::mat4> mats = instancesFromString(instances);

        loadRegular(filePath.c_str(), texturePath.c_str(), normalTexturePath.c_str(), mats);
    }
    else if (line._Starts_with("animTex"))
    {
        std::vector<std::string> itms = split(line, ";");
        std::string filePath = itms[1];
        std::string texturePath = itms[2];
        std::string instances = itms[3];

        std::vector<glm::mat4> mats = instancesFromString(instances);

        loadAnimTextureParams(filePath.c_str(), texturePath.c_str(), mats);
    }
}

void Scene::loadParticleSystemParams(const SwChar* texturePath, glm::vec3 pos, glm::vec3 dir, U32 count)
{
    sgfx::ShaderAttributeList attribs = {};
    attribs.mBufferInput = {{sgfx::ShaderBufferInputRate::InputRate_Vertex, sizeof(float) * 3u}};
    attribs.mAttributes = {{0u, sgfx::ShaderAttributeDataType::vec3f, 0u}};
    attribs.mDescriptors = {
        {sgfx::DescriptorType::TextureSampler,
         sgfx::Count(1u),
         {sgfx::StageType::fragmentStage, sgfx::StageType::tessellationStage}},
    };
    attribs.mPushConstantSize = sizeof(glm::mat4) * 2u;
    attribs.mEnableDepthTest = true;
    attribs.mEnableBlending = false;
    attribs.mEnableDepthWrite = true;
    attribs.mPrimitiveType = sgfx::PrimitiveType::point;

    auto shader = mCompositor->createShader(1u, attribs);
    mAssetManager->loadShader(shader, "AoG/shaders/particle.shader");

    auto texture = mAssetManager->loadTexture(texturePath);

    mRenderables.emplace_back(common::CreateRef<ParticleSystem>(mContext, count, pos, dir, texture, shader));
}

void Scene::loadAnimTextureParams(const SwChar* meshFile, const SwChar* texturePath, std::vector<glm::mat4>& inst)
{
    auto asset = mAssetManager->loadMesh(meshFile, false);

    common::Resource<sgfx::Buffer> instBuffer = createInstanceBuffer(inst);

    sgfx::ShaderAttributeList attribs = {};
    attribs.mBufferInput = {{sgfx::ShaderBufferInputRate::InputRate_Vertex, sizeof(float) * (3u + 3u + 2u)},
                            {sgfx::ShaderBufferInputRate::InputRate_Instance, sizeof(float) * (16u)}};
    attribs.mAttributes = {
        {0u, sgfx::ShaderAttributeDataType::vec3f, 0u},
        {0u, sgfx::ShaderAttributeDataType::vec3f, sizeof(float) * 3u},
        {0u, sgfx::ShaderAttributeDataType::vec2f, sizeof(float) * 6u},
        {1u, sgfx::ShaderAttributeDataType::vec4f, 0u},
        {1u, sgfx::ShaderAttributeDataType::vec4f, sizeof(float) * 4u},
        {1u, sgfx::ShaderAttributeDataType::vec4f, sizeof(float) * 8u},
        {1u, sgfx::ShaderAttributeDataType::vec4f, sizeof(float) * 12u},
    };
    attribs.mDescriptors = {
        {sgfx::DescriptorType::UniformBuffer, sgfx::Count(1u), {sgfx::StageType::vertexStage}},
        {sgfx::DescriptorType::TextureSampler, sgfx::Count(1u), {sgfx::StageType::fragmentStage}},
    };
    attribs.mPushConstantSize = sizeof(glm::mat4) * 2u;
    attribs.mEnableDepthTest = true;
    attribs.mEnableBlending = false;
    attribs.mEnableDepthWrite = true;
    attribs.mPrimitiveType = sgfx::PrimitiveType::triangle;

    auto shader = mCompositor->createShader(1u, attribs);
    mAssetManager->loadShader(shader, "AoG/shaders/animatedTexture.shader");

    auto texture = mAssetManager->loadTexture(texturePath);

    mRenderables.emplace_back(common::CreateRef<AnimatedTextureMesh>(mContext, asset, instBuffer, texture, shader));
}

void Scene::loadRegular(const SwChar* meshFile, const SwChar* diffuseTexture, const SwChar* normalTexture,
                        std::vector<glm::mat4>& inst)
{
    auto asset = mAssetManager->loadMesh(meshFile, false);

    common::Resource<sgfx::Buffer> instBuffer = createInstanceBuffer(inst);

    sgfx::ShaderAttributeList attribs = {};
    attribs.mBufferInput = {{sgfx::ShaderBufferInputRate::InputRate_Vertex, sizeof(float) * (3u + 3u + 2u)},
                            {sgfx::ShaderBufferInputRate::InputRate_Instance, sizeof(float) * (16u)}};
    attribs.mAttributes = {
        {0u, sgfx::ShaderAttributeDataType::vec3f, 0u},
        {0u, sgfx::ShaderAttributeDataType::vec3f, sizeof(float) * 3u},
        {0u, sgfx::ShaderAttributeDataType::vec2f, sizeof(float) * 6u},
        {1u, sgfx::ShaderAttributeDataType::vec4f, 0u},
        {1u, sgfx::ShaderAttributeDataType::vec4f, sizeof(float) * 4u},
        {1u, sgfx::ShaderAttributeDataType::vec4f, sizeof(float) * 8u},
        {1u, sgfx::ShaderAttributeDataType::vec4f, sizeof(float) * 12u},
    };
    attribs.mDescriptors = {
        {sgfx::DescriptorType::UniformBuffer, sgfx::Count(1u), {sgfx::StageType::vertexStage}},
        {sgfx::DescriptorType::TextureSampler, sgfx::Count(1u), {sgfx::StageType::fragmentStage}},
        {sgfx::DescriptorType::TextureSampler, sgfx::Count(1u), {sgfx::StageType::fragmentStage}},
    };
    attribs.mPushConstantSize = sizeof(glm::mat4) * 2u;
    attribs.mEnableDepthTest = true;
    attribs.mEnableBlending = false;
    attribs.mEnableDepthWrite = true;
    attribs.mPrimitiveType = sgfx::PrimitiveType::triangle;

    auto shader = mCompositor->createShader(1u, attribs);
    mAssetManager->loadShader(shader, "AoG/shaders/regular.shader");

    auto diffuse = mAssetManager->loadTexture(diffuseTexture);
    auto normal = mAssetManager->loadTexture(normalTexture);
    auto optNormal = mAssetManager->loadTexture("AoG/textures/neutral.png");

    mRenderables.emplace_back(
        common::CreateRef<RegularMesh>(mContext, asset, instBuffer, diffuse, normal, optNormal, shader, shader));
}