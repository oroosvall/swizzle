
#include "Game.hpp"

#include <glm/glm.hpp>

#include "swizzle/input/Input.hpp"

Game::Game()
    : cam(glm::radians(45.0F), 1280, 720)
    , mMesh(nullptr)
    , mController(cam)
{
    mWindow = sw::core::CreateWindow(1920, 1080, "Sandbox");
    mWindow->show();

    sw::input::SetInputSource(mWindow);

    mSwapchain = sw::gfx::CreateSwapchain(mWindow);
    //mSwapchain->setVsync(sw::gfx::VSyncTypes::vSyncAdaptive);

    mCmdBuffer = sw::gfx::CreateCommandBuffer();

     sw::gfx::ShaderBufferInput bufferInput[] = { sw::gfx::ShaderBufferInputRate::InputRate_Vertex, sizeof(float) * (3U + 3U + 2U) };

    sw::gfx::ShaderAttribute attributes[] = {
        { 0U, sw::gfx::ShaderAttributeDataType::vec3_24, 0U},
        { 0U, sw::gfx::ShaderAttributeDataType::vec3_24, sizeof(float) * 3U },
        { 0U, sw::gfx::ShaderAttributeDataType::vec2_16, sizeof(float) * 6U }
    };

    sw::gfx::ShaderAttributeList attribs;
    attribs.mNumBuffers = 1U;
    attribs.mBufferInput = bufferInput;
    attribs.mNumAttributes = 3U;
    attribs.mAttributes = attributes;

    mShader = mSwapchain->createShader(attribs);
    mShader->load("shaders/simple.shader");

    mBuffer = sw::gfx::CreateBuffer(sw::gfx::BufferType::Vertex);

    mTexture = sw::gfx::CreateTexture();

    float bla[] = {
        0.50F, 0.50F, 0.0F, //0.0, 0.0, 1.0, 1.0, 0.0,
        -0.5F, 0.50F, 0.0F, //0.0, 0.0, 1.0, 0.0, 0.0,
        0.50F, -0.5F, 0.0F, //0.0, 0.0, 1.0, 1.0, 1.0,
                            //
        0.50F, -0.5F, 0.0F, //0.0, 0.0, 1.0, 1.0, 1.0,
        -0.5F, 0.50F, 0.0F, //0.0, 0.0, 1.0, 0.0, 0.0,
        -0.5F, -0.5F, 0.0F, //0.0, 0.0, 1.0, 0.0, 1.0,
    };

    mBuffer->setBufferData(bla, sizeof(bla), sizeof(float) * 3U);

    cam.setPosition({ 0.0F, 0.0F, 5.5F });

    mMesh = new Model();
    mMesh->load("C:/tmp/rock.obj");

    size_t size = mMesh->mMeshes.back().mTriangles.size() * 3 * (3 + 3 + 2);

    char* data = new char[size * sizeof(float)];
        
    size_t offset = 0;

    auto triList = mMesh->mMeshes.back().mTriangles;

    for (size_t i = 0; i < triList.size(); i++)
    {
        memcpy(&data[offset], &mMesh->mMeshes.back().mVertices.data()[triList[i].v1], sizeof(float) * 3);
        offset += sizeof(float) * 3;
        memset(&data[offset], 0, sizeof(float) * 3);
        offset += sizeof(float) * 3;
        memset(&data[offset], 0, sizeof(float) * 2);
        offset += sizeof(float) * 2;
        memcpy(&data[offset], &mMesh->mMeshes.back().mVertices.data()[triList[i].v2], sizeof(float) * 3);
        offset += sizeof(float) * 3;
        memset(&data[offset], 0, sizeof(float) * 3);
        offset += sizeof(float) * 3;
        memset(&data[offset], 0, sizeof(float) * 2);
        offset += sizeof(float) * 2;
        memcpy(&data[offset], &mMesh->mMeshes.back().mVertices.data()[triList[i].v3], sizeof(float) * 3);
        offset += sizeof(float) * 3;
        memset(&data[offset], 0, sizeof(float) * 3);
        offset += sizeof(float) * 3;
        memset(&data[offset], 0, sizeof(float) * 2);
        offset += sizeof(float) * 2;
    }

    mBuffer->setBufferData(data, size * sizeof(F32), sizeof(float) * (3U + 3U + 2U));

 }

Game::~Game()
{
    delete mMesh;
}

bool Game::update(float dt)
{
    swizzle::input::InputFrameReset();

    mWindow->pollEvents();

    mController.update(dt);

    U32 x, y;
    mWindow->getSize(x, y);

    sw::gfx::HSVA hsv = sw::gfx::RgbaToHsva(rgba);
    hsv.h += 20.0F * dt;
    rgba = sw::gfx::HsvaToRgba(hsv);

    mSwapchain->setClearColor({ rgba.r, rgba.g, rgba.b, rgba.a });

    struct tmp
    {
        glm::mat4 model;
        glm::mat4 view;
        glm::mat4 proj;
        glm::vec4 eye;
    };

    tmp t = {};
    t.model = glm::mat4(1.0F);
    t.view = cam.getView();
    t.proj = cam.getProjection();
    t.eye = glm::vec4(cam.getPosition(), 1.0);

    mSwapchain->prepare();
    mCmdBuffer->begin();
    mCmdBuffer->beginRenderPass(mSwapchain);

    mCmdBuffer->bindShader(mShader);
    mCmdBuffer->setViewport(x, y);

    {
        mCmdBuffer->setShaderConstant(mShader, (U8*)&t, sizeof(t));
        mCmdBuffer->draw(mBuffer);
    }

    mCmdBuffer->endRenderPass();
    mCmdBuffer->end();
    mCmdBuffer->submit(mSwapchain);
    mSwapchain->present();

    return mWindow->isVisible();
}
