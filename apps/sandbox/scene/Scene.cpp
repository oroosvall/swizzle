
/* Include files */

#include "Scene.hpp"

#include <fstream>
#include <iostream>

#include "Animated.hpp"
#include "Sky.hpp"

#include <glm/gtc/matrix_transform.hpp>

#include <algorithm>
#include <swizzle/asset2/Assets.hpp>

/* Defines */

/* Typedefs */

/* Structs/Classes */

/* Static Variables */

/* Static Function Declaration */

/* Static Function Definition */

/* Function Definition */

/* Class Public Function Definition */

Scene::Scene(common::Resource<swizzle::gfx::GfxDevice> dev, common::Resource<AssetManager> assetManager,
             common::Resource<Compositor> compositor)
    : mDevice(dev)
    , mAssetManager(assetManager)
    , mCompositor(compositor)
    , mSceneState(SceneState::NotLoaded)
{
}

Scene::~Scene() {}

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
    attribsSky.mEnableDepthWrite = false;
    attribsSky.mEnableBlending = false;
    attribsSky.mPrimitiveType = swizzle::gfx::PrimitiveType::triangle;

    skyShader = mCompositor->createShader(0u, attribsSky);
    skyShader->load("shaders/sky.shader");

    skyMaterial = mDevice->createMaterial(skyShader, swizzle::gfx ::SamplerMode::SamplerModeClamp);

    auto skyData = mAssetManager->loadCubeTexture("texture/right.png", "texture/left.png", "texture/top.png",
                                                  "texture/bottom.png", "texture/front.png", "texture/back.png");

    skyTexture = mDevice->createCubeMapTexture(skyData->getWidth(), skyData->getHeight(), 4u);
    skyMaterial->setDescriptorTextureResource(0, skyTexture);

    std::function<void(common::Unique<swizzle::gfx::CommandTransaction> & trans)> doStuff =
        [this, skyData, skyTexture](common::Unique<swizzle::gfx::CommandTransaction>& trans) {
            auto buffer =
                mDevice->createBuffer(swizzle::gfx::GfxBufferType::UniformBuffer, swizzle::gfx::GfxMemoryArea::Host);
            buffer->setBufferData(skyData->getData()->data(), skyData->getData()->size(), 4u);

            swizzle::gfx::TextureDimensions size{};
            size.mHeight = skyData->getHeight();
            size.mWidth = skyData->getWidth();
            size.mLayers = 6u;
            size.mMipLevels = 1u;

            trans->copyBufferToTexture(skyTexture, buffer, size);
        };

    mTextureUpload.push_back(doStuff);

    swizzle::asset2::MeshAssetLoaderDescription ldi = {};
    ldi.mLoadPossitions = {
        {swizzle::asset2::AttributeTypes::VertexPosition, 0u},
        {swizzle::asset2::AttributeTypes::NormalVector, sizeof(float) * 3u},
        {swizzle::asset2::AttributeTypes::UvCoordinates, sizeof(float) * 6u},
    };

    auto mesh2 = swizzle::asset2::LoadMesh("meshes/inverted_sphere.obj", ldi);

    auto vertexBuffer =
        mDevice->createBuffer(swizzle::gfx::GfxBufferType::Vertex, swizzle::gfx::GfxMemoryArea::DeviceLocalHostVisible);
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

    common::Resource<sgfx::GfxBuffer> verts = mDevice->createBuffer(sgfx::GfxBufferType::Vertex, sgfx::GfxMemoryArea::DeviceLocalHostVisible);
    common::Resource<sgfx::GfxBuffer> idx = mDevice->createBuffer(sgfx::GfxBufferType::Index, sgfx::GfxMemoryArea::DeviceLocalHostVisible);

    verts->setBufferData((U8*)mesh2->getVertexDataPtr(), mesh2->getVertexDataSize(),
                         sizeof(float) * (3u + 3u + 2u + 4u + 4u));
    idx->setBufferData((U8*)mesh2->getIndexDataPtr(), mesh2->getIndexDataSize(), sizeof(U32) * 3u);

    common::Resource<sgfx::GfxBuffer> instBuffer = mDevice->createBuffer(sgfx::GfxBufferType::Vertex, sgfx::GfxMemoryArea::DeviceLocalHostVisible);

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
    attribsAnim.mEnableDepthWrite = true;
    attribsAnim.mEnableBlending = false;
    attribsAnim.mPrimitiveType = sgfx::PrimitiveType::triangle;

    auto textureData = mAssetManager->loadTexture("texture/lightGray.png");

    texture = mDevice->createTexture(textureData->getWidth(), textureData->getHeight(), 4u, false);

    std::function<void(common::Unique<swizzle::gfx::CommandTransaction>& trans)> doStuff =
        [this, textureData, texture](common::Unique<swizzle::gfx::CommandTransaction>& trans) {
        auto buffer =
            mDevice->createBuffer(swizzle::gfx::GfxBufferType::UniformBuffer, swizzle::gfx::GfxMemoryArea::Host);
        buffer->setBufferData(textureData->getData()->data(), textureData->getData()->size(), 4u);

        swizzle::gfx::TextureDimensions size{};
        size.mHeight = textureData->getHeight();
        size.mWidth = textureData->getWidth();
        size.mLayers = 1u;
        size.mMipLevels = 1u;

        trans->copyBufferToTexture(texture, buffer, size);
    };

    mTextureUpload.push_back(doStuff);

    shader = mCompositor->createShader(0u, attribsAnim);
    shader->load("shaders/animated_inst.shader");

    mRenderables.emplace_back(common::CreateRef<Animated>(mDevice, mesh2, instBuffer, texture, shader));
}

SceneState Scene::update(DeltaTime dt, common::Unique<swizzle::gfx::CommandTransaction>& trans)
{
    for (auto& it : mTextureUpload)
    {
        it(trans);
    }

    mTextureUpload.clear();

    for (auto& it : mRenderables)
    {
        it->update(dt, trans);
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
