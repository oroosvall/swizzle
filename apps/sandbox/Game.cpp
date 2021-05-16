
#include "Game.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <swizzle/core/Input.hpp>

#include <swizzle/profile/Profiler.hpp>

#include <swizzle/asset/TextureLoader.hpp>
#include <swizzle/asset/MeshLoader.hpp>

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
    guiLabel = sw::gui::CreateLabel(mGfxContext);
    guiLabel->setText("This is amazing!\n");

    std::string cfgFile = sw::core::GetAppCacheDirectory();
    cfgFile += "engine.cfg";
    mGameCfg.read(cfgFile.c_str());
    utils::Value v;
    v.setString(".");
    mGameCfg.setValue("AssetDir", v);

    mSwapchain->setVsync(sw::gfx::VSyncTypes::vSyncOff);

    mCmdBuffer = mGfxContext->createCommandBuffer(3);

    sw::gfx::ShaderBufferInput bufferInput = { sw::gfx::ShaderBufferInputRate::InputRate_Vertex, sizeof(float) * (3U + 3U + 2U) };

    sw::gfx::ShaderAttribute attributes[] = {
        { 0U, sw::gfx::ShaderAttributeDataType::vec3f, 0U},
        { 0U, sw::gfx::ShaderAttributeDataType::vec3f, sizeof(float) * 3U },
        { 0U, sw::gfx::ShaderAttributeDataType::vec2f, sizeof(float) * 6U }
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

    mTexture = swizzle::asset::LoadTexture2D(mGfxContext, "texture/lightGray.png");
    mMaterial->setDescriptorTextureResource(0u, mTexture);


    sw::gfx::ShaderBufferInput bufferInputText = { sw::gfx::ShaderBufferInputRate::InputRate_Vertex, sizeof(float) * (3U + 2U) };

    sw::gfx::ShaderAttribute attributesText[] = {
        { 0U, sw::gfx::ShaderAttributeDataType::vec3f, 0U},
        { 0U, sw::gfx::ShaderAttributeDataType::vec2f, sizeof(float) * 3U }
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

    mUniformBuffer = mGfxContext->createBuffer(sw::gfx::BufferType::UniformBuffer);

    mUniformBuffer->setBufferData(&lampColor, sizeof(lampColor), 1);
    mMaterial->setDescriptorBufferResource(1u, mUniformBuffer);

    sw::gfx::ShaderBufferInput bufferInputSky = { sw::gfx::ShaderBufferInputRate::InputRate_Vertex, sizeof(float) * (3U + 3U + 2U) };

    sw::gfx::ShaderAttribute attributesSky[] = {
        { 0U, sw::gfx::ShaderAttributeDataType::vec3f, 0U},
        { 0U, sw::gfx::ShaderAttributeDataType::vec3f, sizeof(float) * 3U },
        { 0U, sw::gfx::ShaderAttributeDataType::vec2f, sizeof(float) * 6U }
    };

    sw::gfx::ShaderAttributeList attribsSky = {};
    attribsSky.mNumBuffers = 1U;
    attribsSky.mBufferInput = &bufferInputSky;
    attribsSky.mNumAttributes = COUNT_OF(attributesSky);
    attribsSky.mAttributes = attributesSky;
    attribsSky.mEnableDepthTest = false;
    attribsSky.mEnableBlending = false;

    mSkyShader = mSwapchain->createShader(attribsSky);
    mSkyShader->load("shaders/sky.shader");

    mSkyMaterial = mSkyShader->createMaterial();

    mSkyTexture = sw::asset::LoadTextureCubeMap(mGfxContext,
        "texture/right.png", "texture/left.png", "texture/top.png", "texture/bottom.png", "texture/front.png", "texture/back.png");

    mSkyMaterial->setDescriptorTextureResource(0, mSkyTexture);

    mSkysphere = sw::asset::LoadMesh(mGfxContext, "meshes/inverted_sphere.obj", true);

    // "meshes/main_city.obj"
    // "C:/tmp/cobra.obj"
    // "C:/tmp/dragon.obj"

    mMesh = sw::asset::LoadMesh(mGfxContext, "meshes/main_city.obj", true);

}

SwBool Game::userUpdate(F32 dt)
{
    if (sw::input::WasPressedThisFrame(sw::input::Keys::KeySpace))
    {
        extraText++;
    }

    sw::profile::ProfileEvent(__FUNCTION__, sw::profile::ProfileEventType::EventType_Frame);
    mFpsCounter.tick(dt);

    auto stats = mGfxContext->getStatistics();

    std::string title = "Heljo world\n";

    title += "Frames: " + std::to_string(mSwapchain->getFrameCounter()) + "\n";
    title += "FPS: " + std::to_string(mFpsCounter.getFps()) + "\n\n";

    title += "GFX: Gpu Memory usage: " + std::to_string(stats.mGpuMemoryUsage) + "\n";
    title += "GFX: Cpu Memory usage: " + std::to_string(stats.mCpuMemoryUsage) + "\n";
    title += "Staged objects: " + std::to_string(stats.mStagedObjects) + "\n";
    title += "Num Textures: " + std::to_string(stats.mNumTextures) + "\n";
    title += "Num Buffers: " + std::to_string(stats.mNumBuffers) + "\n";
    title += "Draw call count: " + std::to_string(mCmdBuffer->getDrawCount()) + "\n";
    title += "Vertex count: " + std::to_string(mCmdBuffer->getVertCount()) + "\n";
    title += "Triangle count: " + std::to_string(mCmdBuffer->getTriCount()) + "\n";
    title += "RemainingSize: " + std::to_string(guiLabel->getBuffer()->getRemainingSize()) + "\n";

    for (int i = 0; i < extraText; i++)
    {
        title += "a";
    }

    //mWindow->setTitle(title.c_str());

    guiLabel->setText(title.c_str());

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
    delete guiLabel;

    std::string cfgFile = sw::core::GetAppCacheDirectory();
    cfgFile += "engine.cfg";
    mGameCfg.write(cfgFile.c_str());

    mCmdBuffer.reset();

    mShader.reset();
    mMaterial.reset();

    mTexture.reset();

    mFrameBuffer.reset();

    mUniformBuffer.reset();

    mMesh.mVertexBuffer.reset();
    mMesh.mIndexBuffer.reset();

    mSkysphere.mVertexBuffer.reset();
    mSkysphere.mIndexBuffer.reset();

    mSkyShader.reset();

    mSkyTexture.reset();
    mSkyMaterial.reset();

    mTextShader.reset();
    mTextMaterial.reset();

}

void Game::updateMainWindow()
{
    sw::profile::ProfileEvent(__FUNCTION__, sw::profile::ProfileEventType::EventType_Generic);
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
    mCmdBuffer->uploadTexture(mSkyTexture);

    mCmdBuffer->beginRenderPass(mSwapchain);

    mCmdBuffer->bindShader(mShader);

    mCmdBuffer->bindMaterial(mShader, mMaterial);
    mCmdBuffer->setViewport(x, y);

    mCmdBuffer->setShaderConstant(mShader, (U8*)&t, sizeof(t));

    mCmdBuffer->drawIndexed(mMesh.mVertexBuffer, mMesh.mIndexBuffer);

    // Wormhole stuff

    mCmdBuffer->bindShader(mSkyShader);

    mCmdBuffer->bindMaterial(mSkyShader, mSkyMaterial);
    mCmdBuffer->setViewport(x, y);

    t.model = glm::translate(glm::mat4(1.0F), { 0.0F, 0.0F, 0.0F });

    mCmdBuffer->setShaderConstant(mShader, (U8*)&t, sizeof(t));

    mCmdBuffer->draw(mSkysphere.mVertexBuffer);


    // Text stuff

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

    mCmdBuffer->endRenderPass();
    mCmdBuffer->end();
    mCmdBuffer->submit(mSwapchain);
    mSwapchain->present();
}
