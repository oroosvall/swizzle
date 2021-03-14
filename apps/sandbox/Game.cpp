
#include "Game.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <swizzle/core/Input.hpp>

Game::Game()
    : cam(glm::radians(45.0F), 1280, 720)
    , mController(cam)
{
}

Game::~Game()
{
}

void Game::userSetup()
{
    mSwapchain->setVsync(sw::gfx::VSyncTypes::vSyncOff);

    //mFrameBuffer = mGfxContext->createFrameBuffer();

    mCmdBuffer = mGfxContext->createCommandBuffer();

    sw::gfx::ShaderBufferInput bufferInput = { sw::gfx::ShaderBufferInputRate::InputRate_Vertex, sizeof(float) * (3U + 3U + 2U) };

    sw::gfx::ShaderAttribute attributes[] = {
        { 0U, sw::gfx::ShaderAttributeDataType::vec3_24, 0U},
        { 0U, sw::gfx::ShaderAttributeDataType::vec3_24, sizeof(float) * 3U },
        { 0U, sw::gfx::ShaderAttributeDataType::vec2_16, sizeof(float) * 6U }
    };

    sw::gfx::ShaderAttributeList attribs = {};
    attribs.mNumBuffers = 1U;
    attribs.mBufferInput = &bufferInput;
    attribs.mNumAttributes = COUNT_OF(attributes);
    attribs.mAttributes = attributes;

    mShader = mSwapchain->createShader(attribs);
    mShader->load("shaders/simple.shader");

    //mTexture = mGfxContext->createTexture(1024U, 1024U);

    //mTexture->setData(2, 2, 4, singlePixel);

    cam.setPosition({ 0.0F, 0.0F, 5.5F });

    Model* mMesh = new Model();
    mMesh->load("meshes/main_city.obj");

    mUniformBuffer = mGfxContext->createBuffer(sw::gfx::BufferType::UniformBuffer);

    mUniformBuffer->setBufferData(&lampColor, sizeof(lampColor), 1);

    for (auto& m : mMesh->mMeshes)
    {
        auto buffer = mGfxContext->createBuffer(sw::gfx::BufferType::Vertex);

        size_t size = m.mTriangles.size() * 3 * (3 + 3 + 2);

        char* data = new char[size * sizeof(float)];

        size_t offset = 0;

        auto& mesh = m;
        auto verts = mesh.mVertices.data();
        auto normals = mesh.mNormals.data();
        auto uvs = mesh.mUvs.data();
        auto& triList = mesh.mTriangles;

        for (auto& tri : triList)
        {
            memcpy(&data[offset], &verts[tri.v1], sizeof(float) * 3);
            offset += sizeof(float) * 3;
            memcpy(&data[offset], &normals[tri.v1], sizeof(float) * 3);
            offset += sizeof(float) * 3;
            memcpy(&data[offset], &uvs[tri.v1], sizeof(float) * 2);
            offset += sizeof(float) * 2;
            memcpy(&data[offset], &verts[tri.v2], sizeof(float) * 3);
            offset += sizeof(float) * 3;
            memcpy(&data[offset], &normals[tri.v2], sizeof(float) * 3);
            offset += sizeof(float) * 3;
            memcpy(&data[offset], &uvs[tri.v2], sizeof(float) * 2);
            offset += sizeof(float) * 2;
            memcpy(&data[offset], &verts[tri.v3], sizeof(float) * 3);
            offset += sizeof(float) * 3;
            memcpy(&data[offset], &normals[tri.v3], sizeof(float) * 3);
            offset += sizeof(float) * 3;
            memcpy(&data[offset], &uvs[tri.v3], sizeof(float) * 2);
            offset += sizeof(float) * 2;
        }

        buffer->setBufferData(data, size * sizeof(F32), sizeof(float) * (3U + 3U + 2U));
        delete[] data;

        mBuffers.push_back(buffer);
    }

    delete mMesh;
    mMesh = nullptr;
}

SwBool Game::userUpdate(F32 dt)
{
    mController.update(dt);

    sw::gfx::HSVA hsv = sw::gfx::RgbaToHsva(rgba);
    hsv.h += 20.0F * dt;
    rgba = sw::gfx::HsvaToRgba(hsv);

    sw::gfx::HSVA hsvLamp = sw::gfx::RgbaToHsva(lampColor);
    hsvLamp.h += 20.0F * dt;
    lampColor = sw::gfx::HsvaToRgba(hsvLamp);

    updateMainWindow();

    return mWindow->isVisible();
}

void Game::userCleanup()
{
    mCmdBuffer.reset();
    mUniformBuffer.reset();

    mShader.reset();
    mBuffers.clear();

    mTexture.reset();

    mFrameBuffer.reset();
}

void Game::updateMainWindow()
{
    U32 x, y;
    mWindow->getSize(x, y);

    cam.changeAspect((F32)x, (F32)y);

    void* mem = mUniformBuffer->mapMemory(sizeof(lampColor));
    memcpy(mem, &lampColor, sizeof(lampColor));
    mUniformBuffer->unmapMemory();

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
    t.eye = glm::vec4(cam.getPosition(), 1.0F);

    t.model = glm::translate(t.model, { 0, -10, -10 });

    mSwapchain->prepare();
    mCmdBuffer->begin();
    mCmdBuffer->beginRenderPass(mSwapchain);

    mCmdBuffer->bindShader(mShader);

    mCmdBuffer->bindMaterial(mShader, mUniformBuffer);
    mCmdBuffer->setViewport(x, y);

    for (auto& buffer : mBuffers)
    {
        mCmdBuffer->setShaderConstant(mShader, (U8*)&t, sizeof(t));
        mCmdBuffer->draw(buffer);
    }

    mCmdBuffer->endRenderPass();
    mCmdBuffer->end();
    mCmdBuffer->submit(mSwapchain);
    mSwapchain->present();
}
