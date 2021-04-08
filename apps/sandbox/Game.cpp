
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
    delete guiLabel;
}

void Game::userSetup()
{
    guiLabel = sw::gui::CreateLabel(mGfxContext);
    guiLabel->setText("This is amazing!\n");

    std::string cfgFile = sw::core::GetAppCacheDirectory();
    cfgFile += "engine.cfg";
    mGameCfg.read(cfgFile.c_str());
    utils::Value v;
    v.setString(".");
    mGameCfg.setValue("AssetDir", v);

    mSwapchain->setVsync(sw::gfx::VSyncTypes::vSyncOff);

    //mFrameBuffer = mGfxContext->createFrameBuffer();

    mCmdBuffer = mGfxContext->createCommandBuffer(3);

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
    attribs.mEnableDepthTest = true;
    attribs.mEnableBlending = false;

    mShader = mSwapchain->createShader(attribs);
    mShader->load("shaders/simple.shader");

    mMaterial = mShader->createMaterial();

    mTexture = mGfxContext->createTexture(100, 100);
    mMaterial->setDescriptorTextureResource(0u, mTexture);


    sw::gfx::ShaderBufferInput bufferInputText = { sw::gfx::ShaderBufferInputRate::InputRate_Vertex, sizeof(float) * (3U + 2U) };

    sw::gfx::ShaderAttribute attributesText[] = {
        { 0U, sw::gfx::ShaderAttributeDataType::vec3_24, 0U},
        { 0U, sw::gfx::ShaderAttributeDataType::vec2_16, sizeof(float) * 3U }
    };

    sw::gfx::ShaderAttributeList attribsText = {};
    attribsText.mNumBuffers = 1U;
    attribsText.mBufferInput = &bufferInputText;
    attribsText.mNumAttributes = COUNT_OF(attributesText);
    attribsText.mAttributes = attributesText;
    attribsText.mEnableDepthTest = false;
    attribsText.mEnableBlending = true;

    mTextShader = mSwapchain->createShader(attribsText);
    mTextShader->load("shaders/text.shader");
    mTextMaterial = mShader->createMaterial();
    mTextMaterial->setDescriptorTextureResource(0u, guiLabel->getTexture());

    //mTexture = mGfxContext->createTexture(1024U, 1024U);

    //mTexture->setData(2, 2, 4, singlePixel);

    cam.setPosition({ 0.0F, 0.0F, 5.5F });

    Model* mesh = new Model();
    mesh->load("meshes/main_city.obj");
    //mesh->load("C:/tmp/cobra.obj");
    //mesh->load("C:/tmp/dragon.obj");

    mUniformBuffer = mGfxContext->createBuffer(sw::gfx::BufferType::UniformBuffer);

    mUniformBuffer->setBufferData(&lampColor, sizeof(lampColor), 1);
    mMaterial->setDescriptorBufferResource(1u, mUniformBuffer);

    U32 triangleSum = 0u;
    U32 vertSum = 0u;

    for (auto& m : mesh->mMeshes)
    {
        auto buffer = mGfxContext->createBuffer(sw::gfx::BufferType::Vertex);

        size_t size = m.mTriangles.size() * 3 * (3 + 3 + 2);

        char* data = new char[size * sizeof(float)];

        size_t offset = 0;

        auto& mesh1 = m;
        auto verts = mesh1.mVertices.data();
        auto normals = mesh1.mNormals.data();
        auto uvs = mesh1.mUvs.data();
        auto& triList = mesh1.mTriangles;

        triangleSum += (U32)triList.size();
        vertSum += (U32)mesh1.mVertices.size();

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

    printf("Rendering total of %d Triangles\n", triangleSum);
    printf("Rendering total of %d Vertices\n", vertSum);

    delete mesh;
    mesh = nullptr;
}

SwBool Game::userUpdate(F32 dt)
{
    mFpsCounter.tick(dt);
    std::string title = "Heljo world, Fps: ";

    title += std::to_string(mFpsCounter.getFps());
    mWindow->setTitle(title.c_str());

    // guiLabel->setText(title.c_str());

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
    std::string cfgFile = sw::core::GetAppCacheDirectory();
    cfgFile += "engine.cfg";
    mGameCfg.write(cfgFile.c_str());

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

    mCmdBuffer->uploadTexture(mTexture);
    mCmdBuffer->uploadTexture(guiLabel->getTexture());

    mCmdBuffer->beginRenderPass(mSwapchain);

    mCmdBuffer->bindShader(mShader);

    mCmdBuffer->bindMaterial(mShader, mMaterial);
    mCmdBuffer->setViewport(x, y);

    for (auto& buffer : mBuffers)
    {
        mCmdBuffer->setShaderConstant(mShader, (U8*)&t, sizeof(t));
        mCmdBuffer->draw(buffer);
    }

    mCmdBuffer->bindShader(mTextShader);

    mCmdBuffer->setViewport(x, y);

#pragma pack(push, 1)
    struct Gui
    {
        glm::mat4 mat;
        glm::vec3 offest;
    };

#pragma pack(pop)
    Gui g;

    g.mat = glm::orthoRH_ZO(0.0F, (F32)x, (F32)y, 0.0F, 0.01F, 10.0F);
    g.offest = { 5.0F, 50.0F, 0.0F };

    mCmdBuffer->setShaderConstant(mTextShader, (U8*)&g, sizeof(g));
    mCmdBuffer->bindMaterial(mShader, mTextMaterial);

    mCmdBuffer->draw(guiLabel->getBuffer());

    /*t.model = glm::translate(t.model, { 0, 10, 0 });

    for (auto& buffer : mBuffers)
    {
        mCmdBuffer->setShaderConstant(mShader, (U8*)&t, sizeof(t));
        mCmdBuffer->draw(buffer);
    }*/

    mCmdBuffer->endRenderPass();
    mCmdBuffer->end();
    mCmdBuffer->submit(mSwapchain);
    mSwapchain->present();
}
