
#include "Game.hpp"

#pragma warning(push)
#pragma warning(disable : 4201)
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
#pragma warning(pop)

#include <swizzle/core/Input.hpp>

#include <swizzle/asset/TextureLoader.hpp>

#include <utils/StringUtils.hpp>

#include <imgui/imgui.h>
#include <imgui/imgui_impl_swizzle.hpp>

#include <random>

#include "Stats.hpp"

#include "DayOptions.hpp"

float lerp(float a, float b, float f)
{
    return a + f * (b - a);
}

Game::Game()
    : cam(glm::radians(45.0F), 1280, 720)
    , mController(cam)
    , mInputLocked(false)
{
}

Game::~Game() {}

void Game::userSetup()
{
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGui_ImplSwizzle_Init(mGfxContext, mWindow);

    if (mSwapchain->isVsyncModeSupported(sw::gfx::VSyncTypes::vSyncOn))
        mVSyncOptions.push_back(sw::gfx::VSyncTypes::vSyncOn);
    if (mSwapchain->isVsyncModeSupported(sw::gfx::VSyncTypes::vSyncAdaptive))
        mVSyncOptions.push_back(sw::gfx::VSyncTypes::vSyncAdaptive);
    if (mSwapchain->isVsyncModeSupported(sw::gfx::VSyncTypes::vSyncOff))
        mVSyncOptions.push_back(sw::gfx::VSyncTypes::vSyncOff);

    mSwapchain->setVsync(sw::gfx::VSyncTypes::vSyncOn);
    // mSwapchain->setVsync(sw::gfx::VSyncTypes::vSyncOff);

    mCmdBuffer = mGfxContext->createCommandBuffer(2);
    mGfxContext->enablePipelineStatistics(true);
    mGfxContext->enableGpuTiming(true);

    cam.setPosition({0.0F, 0.0F, 5.5F});

    swizzle::gfx::FrameBufferCreateInfo info{};
    info.mDepthType = swizzle::gfx::FrameBufferDepthType::DepthStencil;
    info.mSwapCount = 3u;
    info.mColorAttachFormats = {
        swizzle::gfx::FrameBufferAttachmentType::Srgb,
        swizzle::gfx::FrameBufferAttachmentType::Srgb,
        swizzle::gfx::FrameBufferAttachmentType::F32,
        swizzle::gfx::FrameBufferAttachmentType::F32,
    };
    mWindow->getSize(info.mWidth, info.mHeight);

    mGBuffer = mGfxContext->createFramebuffer(info);
    mGBuffer->setColorAttachmentClearColor(0u, {0.0f, 0.0f, 0.0f, 1.0f});
    mGBuffer->setDepthAttachmentClearValue(1.0f, 0u);

    sw::gfx::ShaderAttributeList attribFsq = {};
    attribFsq.mDescriptors = {
        {sw::gfx::DescriptorType::TextureSampler, sw::gfx::Count(1u), {sw::gfx::StageType::fragmentStage}},
        {sw::gfx::DescriptorType::TextureSampler, sw::gfx::Count(1u), {sw::gfx::StageType::fragmentStage}},
        {sw::gfx::DescriptorType::TextureSampler, sw::gfx::Count(1u), {sw::gfx::StageType::fragmentStage}},
        {sw::gfx::DescriptorType::TextureSampler, sw::gfx::Count(1u), {sw::gfx::StageType::fragmentStage}},
        {sw::gfx::DescriptorType::TextureSampler, sw::gfx::Count(1u), {sw::gfx::StageType::fragmentStage}},
        {sw::gfx::DescriptorType::TextureSampler, sw::gfx::Count(1u), {sw::gfx::StageType::fragmentStage}},
        {sw::gfx::DescriptorType::UniformBuffer, sw::gfx::Count(1u), {sw::gfx::StageType::fragmentStage}},
        {sw::gfx::DescriptorType::TextureSampler, sw::gfx::Count(1u), {sw::gfx::StageType::fragmentStage}},
        {sw::gfx::DescriptorType::TextureSampler, sw::gfx::Count(1u), {sw::gfx::StageType::fragmentStage}},
        {sw::gfx::DescriptorType::UniformBuffer, sw::gfx::Count(1u), {sw::gfx::StageType::fragmentStage}},
    };
    attribFsq.mPushConstantSize = sizeof(glm::mat4) * 2;
    attribFsq.mEnableBlending = true;
    attribFsq.mPrimitiveType = sw::gfx::PrimitiveType::triangle;

    mFsq = mGfxContext->createShader(mSwapchain, sw::gfx::ShaderType::ShaderType_Graphics, attribFsq);
    mFsq->load("AoG/shaders/fsq.shader");

    mFsqMat = mGfxContext->createMaterial(mFsq, swizzle::gfx::SamplerMode::SamplerModeClampEdge);
    ImGui_ImplSwizzle_SetMaterial(mFsqMat);
    mFsqMat->setDescriptorTextureResource(1u, mGBuffer->getColorAttachment(0u));
    mFsqMat->setDescriptorTextureResource(2u, mGBuffer->getDepthAttachment());
    mFsqMat->setDescriptorTextureResource(3u, mGBuffer->getColorAttachment(1u));
    mFsqMat->setDescriptorTextureResource(4u, mGBuffer->getColorAttachment(2u));
    mFsqMat->setDescriptorTextureResource(5u, mGBuffer->getColorAttachment(3u));

    mSceneSettings.mMeshShaders = mGfxContext->hasMeshShaderSupport();
    mSceneSettings.mParticles = false;
    mSceneSettings.mEnableNormalMaps = false;
    mSceneSettings.mEnableAnimatedTextures = false;
    mSceneSettings.mForceLowestMipLayer = false;
    mSceneSettings.mHeightMap = false;
    mSceneSettings.mMipMaps = false;
    mSceneSettings.mTesselation = false;

    mSceneSettings.mSkyInfo.mDaySkyColor = glm::vec4(0.3f, 0.55f, 0.8f, 1.0f);
    mSceneSettings.mSkyInfo.mNightSkyColor = glm::vec4(0.024422f, 0.088654f, 0.147314f, 1.0f);
    mSceneSettings.mSkyInfo.mSun1Color = glm::vec4(1.0f, 0.7f, 0.4f, 1.0f);
    mSceneSettings.mSkyInfo.mSun2Color = glm::vec4(1.0f, 0.8f, 0.5f, 1.0f);
    mSceneSettings.mSkyInfo.mMoon1Color = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);
    mSceneSettings.mSkyInfo.mMoon2Color = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);
    mSceneSettings.mSkyInfo.mSunMoonDir = glm::vec4(1.0, 0.0, 0.0, 0.0f);

    mSceneSettings.mAssetSlowLoad = false;
    mSceneSettings.mReflections = true;
    mSceneSettings.mPortals = true;

    mDayOptions.mShowShaderEditor = false;
    mDayOptions.mEnableDof = false;
    mDayOptions.mDoFFocalPoint = 40.0f;
    mDayOptions.mDoFFocalScale = 200.0f;
    mDayOptions.mGlow = false;
    mDayOptions.mHasMeshShaderSupport = mGfxContext->hasMeshShaderSupport();
    mDayOptions.mBesierCurves = false;
    mDayOptions.mBesierCurvesEditor = false;
    mDayOptions.mCameraPath = false;
    mDayOptions.mCameraTime = 0.0f;
    mDayOptions.mDithering = false;
    mDayOptions.mSkyTime = 0.0f;
    mDayOptions.mCountSkyTime = 0.0;
    mDayOptions.mLensFlare = false;
    mDayOptions.mFlarePos = glm::vec2(0.0f, 0.0f);
    mDayOptions.mTextureViewer = 0;
    mDayOptions.mViewFromLight = false;

    sw::gfx::ShaderAttributeList bezierAttribs = {};
    bezierAttribs.mAttributes = {
        {0u, sw::gfx::ShaderAttributeDataType::vec3f, 0u},
    };
    bezierAttribs.mBufferInput = {
        {sw::gfx::ShaderBufferInputRate::InputRate_Vertex, sizeof(float) * (3u)},
    };
    bezierAttribs.mDescriptors = {};
    bezierAttribs.mPushConstantSize = sizeof(glm::mat4) + sizeof(glm::vec3);
    bezierAttribs.mEnableBlending = false;
    bezierAttribs.mEnableDepthTest = true;
    bezierAttribs.mPrimitiveType = sw::gfx::PrimitiveType::line;

    mBezierShader = mGfxContext->createShader(mGBuffer, sw::gfx::ShaderType::ShaderType_Graphics, bezierAttribs);
    mBezierShader->load("AoG/shaders/lines.shader");

    mCurves.push_back(BezierCurve{mGfxContext, "Camera path"});
    mCurves.back().load("AoG/CameraPath.txt");
    mCurves.push_back(BezierCurve{mGfxContext, "Camera lookat"});
    mCurves.back().load("AoG/CameraLook.txt");

    mCameraPathIndex = 0u;
    mCameraPathIndex = 1u;

    mSkyCycleTime = 120.0f;
    mTime = 0.0f;

    mTextureViewerMat = ImGui_ImplSwizzle_CreateMaterial(mGfxContext);
    mTextureViewerMat->setDescriptorTextureResource(0u, mGBuffer->getColorAttachment(0u));

    std::default_random_engine rndEngine((unsigned)time(nullptr));
    std::uniform_real_distribution<float> rndDist(0.0f, 1.0f);

    // Sample kernel
    std::vector<glm::vec4> ssaoKernel(64);
    for (uint32_t i = 0; i < 64; ++i)
    {
        glm::vec3 sample(rndDist(rndEngine) * 2.0 - 1.0, rndDist(rndEngine) * 2.0 - 1.0, rndDist(rndEngine));
        sample = glm::normalize(sample);
        sample *= rndDist(rndEngine);
        float scale = float(i) / float(64);
        scale = lerp(0.1f, 1.0f, scale * scale);
        ssaoKernel[i] = glm::vec4(sample * scale, 0.0f);
    }

    mSampleBuffer = mGfxContext->createBuffer(swizzle::gfx::BufferType::UniformBuffer);
    mSampleBuffer->setBufferData(ssaoKernel.data(), ssaoKernel.size() * sizeof(glm::vec4), sizeof(glm::vec4));
    mFsqMat->setDescriptorBufferResource(6u, mSampleBuffer, ssaoKernel.size() * sizeof(glm::vec4));

    std::vector<glm::vec4> ssaoNoise(4 * 4);
    for (uint32_t i = 0; i < static_cast<uint32_t>(ssaoNoise.size()); i++)
    {
        ssaoNoise[i] = glm::vec4(rndDist(rndEngine) * 2.0f - 1.0f, rndDist(rndEngine) * 2.0f - 1.0f, 0.0f, 0.0f);
    }

    mNoiseTexture = mGfxContext->createTexture(4, 4, 4, true, (U8*)ssaoNoise.data());
    mFsqMat->setDescriptorTextureResource(7u, mNoiseTexture);

    cam.setPosition({1.0f, 0.75f, 0.0f});
    cam.setRotation(glm::vec3(0.0f, 90.0f, 0.0f));

    swizzle::gfx::FrameBufferCreateInfo shadowMapInfo{};
    shadowMapInfo.mDepthType = swizzle::gfx::FrameBufferDepthType::DepthStencil;
    shadowMapInfo.mSwapCount = 3u;
    shadowMapInfo.mHeight = 2048u;
    shadowMapInfo.mWidth = 2048u;
    shadowMapInfo.mColorAttachFormats = {};

    mShadowMap = mGfxContext->createFramebuffer(shadowMapInfo);
    mShadowMap->setDepthAttachmentClearValue(1.0f, 0u);

    mShadowCamBuffer = mGfxContext->createBuffer(swizzle::gfx::BufferType::UniformBuffer);

    mFsqMat->setDescriptorTextureResource(8u, mShadowMap->getDepthAttachment());

    mCompositor = common::CreateRef<Compositor>(mGfxContext, mSwapchain, mGBuffer, mShadowMap);
    mAssetManager = common::CreateRef<AssetManager>(mGfxContext);
    mScene = common::CreateRef<Scene>(mGfxContext, mCompositor, mAssetManager);

    mShaderEditor = common::CreateRef<ShaderEditor>(mAssetManager);

    mShadowTexturePreview = false;

    mScene->loadSky();
    mScene->loadHeightMap();
    mScene->loadTesselationMesh();
    mScene->loadGlow();
    mScene->loadMeshShader();
    mScene->loadScene("AoG/scene.txt");
    mScene->loadWater();
}

SwBool Game::userUpdate(F32 dt)
{
    comboIdx = 0;
    OPTICK_EVENT("userUpdate");

    mFpsCounter.tick(dt);

    mInputLocked = ImGui::GetIO().WantCaptureMouse || ImGui::GetIO().WantCaptureKeyboard;
    ImGui::GetIO().DeltaTime = dt;

    ImGui_ImplSwizzle_NewFrame(mWindow);
    ImGui::NewFrame();

    std::vector<U64> gpuTimeStamps;

    ImGui::Begin("Statistics");
    {
        OPTICK_EVENT("ImGui::Text");
        std::string text = GetStatisticsText(mGfxContext, mCmdBuffer, mSwapchain, mFpsCounter, gpuTimeStamps);
        ImGui::Text("%s", text.c_str());
    }
    ImGui::End();

    ImGui::Begin("Gfx Timing (Day 18)");
    for (U64 i = 1u; i < gpuTimeStamps.size(); i++)
    {
        U64 zero = gpuTimeStamps[0];
        U64 sample = gpuTimeStamps[i];
        ImGui::Text("Renderpass time (%llu): dt %llu us, tot %llu us", i - 1, (sample - gpuTimeStamps[i - 1]) / 1000ull,
                    (sample - zero) / 1000ull);
    }
    // ImGui::PlotHistogram("Test");
    ImGui::End();

    auto cmb = [](void* data, S32 idx, const char** outText) -> bool {
        sw::gfx::VSyncTypes* pData = (sw::gfx::VSyncTypes*)data;

        switch (pData[idx])
        {
        case sw::gfx::VSyncTypes::vSyncAdaptive:
            *outText = "Vsync Adaptive";
            return true;
        case sw::gfx::VSyncTypes::vSyncOff:
            *outText = "Vsync Off";
            return true;
        case sw::gfx::VSyncTypes::vSyncOn:
            *outText = "Vsync On";
            return true;
        default:
            break;
        }
        return false;
    };

    ImGui::Begin("Options");
    ImGui::Text("Vsync: ");
    ImGui::SameLine();
    if (ImGui::Combo("##0", &mSelectedOption, cmb, (void*)mVSyncOptions.data(), (S32)mVSyncOptions.size(), -1))
    {
        mSwapchain->setVsync(mVSyncOptions[mSelectedOption]);
    }

    DrawDayOptions(mSceneSettings, mDayOptions);

    ImGui::Text("Camera path curve:");
    ImGui::SameLine();
    imguiCurveComboSelect(&mCameraPathIndex);
    ImGui::Text("Camera look curve:");
    ImGui::SameLine();
    imguiCurveComboSelect(&mCameraLookIndex);

    ImGui::End();

    if (mDayOptions.mShowShaderEditor)
    {
        mShaderEditor->render();
    }
    if (mDayOptions.mBesierCurvesEditor)
    {
        if (ImGui::Begin("Besizer curves editor", &mDayOptions.mBesierCurvesEditor))
        {
            ImGui::Text("Curves:");
            imguiCurveComboSelect(&mSelectedCurve);
            ImGui::SameLine();
            if (ImGui::Button("+"))
            {
                mCurves.push_back(BezierCurve{mGfxContext, "Curve"});
            }
            ImGui::SameLine();
            if (ImGui::Button("-"))
            {
                if (mCurves.size() > 0)
                {
                    mCurves.erase(mCurves.begin() + mSelectedCurve);
                    mSelectedCurve = 0u;
                }
            }

            BezierCurve* curve = getCurrentCurve(mSelectedCurve);

            ImGui::Text("Points");
            if (ImGui::BeginListBox("###Points"))
            {
                U32 count = 0u;
                if (curve)
                {
                    auto& ctrlPoints = curve->getControlPoints();
                    for (auto& itm : ctrlPoints)
                    {
                        auto str = glm::to_string(itm) + "##points#" + std::to_string(count);
                        if (ImGui::Selectable(str.c_str(), mBezierItem == count))
                        {
                            mBezierItem = count;
                        }
                        count++;
                    }
                }
                ImGui::EndListBox();
            }

            if (curve)
            {
                SwBool generate = false;
                auto& ctrlPoints = curve->getControlPoints();
                if (ctrlPoints.size() > 0)
                {
                    ImGui::Text("Selected Value:");
                    if (ImGui::InputFloat3("###Selected value", glm::value_ptr(ctrlPoints[mBezierItem])))
                    {
                        generate = true;
                    }
                }
                if (ImGui::Button("Add point"))
                {
                    ctrlPoints.push_back(glm::vec3());
                    generate = true;
                }
                if (ImGui::Button("Remove point"))
                {
                    if (ctrlPoints.size() > 0)
                    {
                        ctrlPoints.erase(ctrlPoints.begin() + mBezierItem);
                        mBezierItem = 0u;
                        generate = true;
                    }
                }
                if (generate)
                {
                    curve->generateCurve();
                }
            }
        }
        ImGui::End();
    }

    if (mDayOptions.mTextureViewer)
    {
        if (ImGui::Begin("Texture viewer", &mDayOptions.mTextureViewer))
        {
            if (ImGui::Checkbox("##textureShadow", &mShadowTexturePreview))
            {
                if (mShadowTexturePreview)
                {
                    mTextureViewerMat->setDescriptorTextureResource(0u, mShadowMap->getDepthAttachment());
                }
                else
                {
                    mTextureViewerMat->setDescriptorTextureResource(0u, mGBuffer->getColorAttachment(mSelectedTexture));
                }
            }
            std::string comboLabel = "##textureViewer";
            std::string textureText = "GBuffer Texture idx " + std::to_string(mSelectedTexture);
            if (ImGui::BeginCombo(comboLabel.c_str(), textureText.c_str()))
            {
                for (size_t i = 0ull; i < mGBuffer->getNumColorAttachments(); ++i)
                {
                    std::string itmText = "GBuffer Texture idx" + std::to_string(i);
                    if (ImGui::Selectable(itmText.c_str(), i == mSelectedTexture))
                    {
                        mSelectedTexture = (U32)i;
                        mTextureViewerMat->setDescriptorTextureResource(0u,
                                                                        mGBuffer->getColorAttachment(mSelectedTexture));
                    }
                }

                ImGui::EndCombo();
            }

            ImGui::Image(&mTextureViewerMat, {800, 800});
        }
        ImGui::End();
    }

    ImGui::EndFrame();

    updateMainWindow(dt);

    return mWindow->isVisible();
}

void Game::userCleanup()
{
    mScene.reset();
    mCompositor.reset();
    mCmdBuffer.reset();
    mGBuffer.reset();
}

void Game::updateMainWindow(F32 dt)
{
    OPTICK_EVENT("Game::updateMainWindow");
    U32 x, y;
    mWindow->getSize(x, y);

    cam.changeAspect((F32)x, (F32)y);

    // mGBuffer->resize(x, y);
    BezierCurve* camPathCurve = getCurrentCurve(mCameraPathIndex);
    BezierCurve* camLookCurve = getCurrentCurve(mCameraLookIndex);
    if (mDayOptions.mCameraPath && camPathCurve && camLookCurve)
    {
        glm::vec3 pos = camPathCurve->getPoint(mDayOptions.mCameraTime);
        glm::vec3 dir = camLookCurve->getPoint(mDayOptions.mCameraTime);
        cam.setPosition(pos);
        cam.lookAt(pos, dir);
    }

    if (!mInputLocked && !mDayOptions.mCameraPath)
    {
        mController.update(dt);
    }

    if (mDayOptions.mCountSkyTime)
        mTime += dt;
    updateSkyTime();

    mSceneSettings.mSkyInfo.mCameraMatrix = cam.getProjection() * cam.getView();
    mSceneSettings.mSkyInfo.mCameraEye = glm::vec4(cam.getPosition(), mSkyTime);

    mSwapchain->setClearColor({0, 0, 0, 1});

    mSwapchain->prepare();

    auto trans = mCmdBuffer->begin();
    trans->uploadTexture(mNoiseTexture);

    mScene->update(dt, mSceneSettings, trans, cam.getPosition());

    imGuiRender(trans);

    auto dTrans = mCmdBuffer->beginRenderPass(mShadowMap, std::move(trans));
    mScene->renderShadows(dTrans, mShadowCam);
    trans = mCmdBuffer->endRenderPass(std::move(dTrans));

    dTrans = mCmdBuffer->beginRenderPass(mGBuffer, std::move(trans));

    mScene->render(dTrans, cam);
    mScene->renderMirror(dTrans, cam);

    if (mDayOptions.mBesierCurves)
    {
        for (auto& c : mCurves)
        {
            if (c.isReady())
            {
                struct bezier
                {
                    glm::mat4 mCam;
                    glm::vec3 mCol;
                } d{};
                d.mCam = cam.getProjection() * cam.getView();
                d.mCol = glm::vec3(0.0f, 1.0f, 0.0f);

                dTrans->bindShader(mBezierShader);
                dTrans->setShaderConstant(mBezierShader, (U8*)&d, sizeof(d));
                dTrans->draw(c.getBuffer());
            }
        }
    }

    trans = mCmdBuffer->endRenderPass(std::move(dTrans));

    dTrans = mCmdBuffer->beginRenderPass(mSwapchain, std::move(trans));

    struct dof
    {
        glm::mat4 mproj;
        glm::vec4 dof;
        float mDofEnabled;
        float mGlowEnabled;
        float mDitherEnabled;
        float mLenseFlareEnabled;
        glm::vec2 mFlarePos;
        glm::vec2 mScreenSize;
        glm::vec3 mSunPos;
    } d{};
    d.mproj = cam.getProjection();
    d.dof.x = mDayOptions.mDoFFocalPoint;
    d.dof.y = mDayOptions.mDoFFocalScale;
    d.dof.z = 1.0f / F32(x);
    d.dof.w = 1.0f / F32(y);
    d.mDofEnabled = mDayOptions.mEnableDof ? 1.0f : 0.0f;
    d.mGlowEnabled = mDayOptions.mGlow ? 1.0f : 0.0f;
    d.mDitherEnabled = mDayOptions.mDithering ? 1.0f : 0.0f;
    d.mLenseFlareEnabled = mDayOptions.mLensFlare ? 1.0f : 0.0f;
    //glm::vec2 lfPos = glm::normalize()
    d.mFlarePos = mDayOptions.mFlarePos;
    d.mScreenSize.x = F32(x);
    d.mScreenSize.y = F32(y);
    d.mSunPos = mSunPos;

    glm::mat4 shadowMat = mShadowCam.getProjection() * mShadowCam.getView();
    mShadowCamBuffer->setBufferData(&shadowMat, sizeof(glm::mat4), sizeof(glm::mat4));
    mFsqMat->setDescriptorBufferResource(9u, mShadowCamBuffer, ~0ull);

    dTrans->bindShader(mFsq);
    dTrans->bindMaterial(mFsq, mFsqMat);
    dTrans->setShaderConstant(mFsq, (U8*)&d, sizeof(d));
    dTrans->drawNoBind(3u, 0u);

    trans = mCmdBuffer->endRenderPass(std::move(dTrans));

    mCmdBuffer->end(std::move(trans));

    mGfxContext->submit(&mCmdBuffer, 1u, mSwapchain);
    mSwapchain->present();
}

void Game::imGuiRender(common::Unique<sw::gfx::CommandTransaction>& trans)
{
    ImGui_ImplSwizzle_UploadFontTexture(trans);

    auto dTrans = mCmdBuffer->beginRenderPass(ImGui_ImplSwizzle_GetFramebuffer(), std::move(trans));
    ImGui::Render();
    ImGui_ImplSwizzle_RenderDrawData(ImGui::GetDrawData(), dTrans);
    trans = mCmdBuffer->endRenderPass(std::move(dTrans));
}

BezierCurve* Game::getCurrentCurve(U32 index)
{
    BezierCurve* curve = nullptr;

    if (mCurves.size() > index)
    {
        curve = &mCurves[index];
    }

    return curve;
}

void Game::imguiCurveComboSelect(U32* curveIndex)
{
    BezierCurve* curve = getCurrentCurve(*curveIndex);
    ImGui::SameLine();
    ImGui::BeginDisabled(curve == nullptr);
    std::string text = "";
    if (curve)
        text = curve->getName();
    std::string comboLabel = "##curve" + std::to_string(comboIdx);
    if (ImGui::BeginCombo(comboLabel.c_str(), text.c_str()))
    {
        for (size_t i = 0ull; i < mCurves.size(); ++i)
        {
            auto& c = mCurves[i];
            std::string itmText = c.getName() + "##" + std::to_string(i);
            if (ImGui::Selectable(itmText.c_str(), text == itmText))
            {
                *curveIndex = (U32)i;
                mBezierItem = 0u;
            }
        }

        ImGui::EndCombo();
    }
    ImGui::EndDisabled();
    comboIdx++;
}

void Game::updateSkyTime()
{
    F32 skyTime = glm::mod(mTime, mSkyCycleTime);
    if (skyTime > mSkyCycleTime)
    {
        skyTime -= mSkyCycleTime;
    }

    mSkyTime = skyTime / mSkyCycleTime;

    const float r = 100.0F;
    const float PI = glm::pi<float>();

    float s = (glm::abs(glm::sin(PI * mSkyTime * 2.0F + (PI * 0.5F))) * 1.6F) - 0.6F;
    float c = glm::cos(PI * mSkyTime * 2.0F + (PI * 0.5F));

    if (mSkyTime >= 0.25F && mSkyTime <= 0.75F)
    {
        c = -c;
    }
    if (s > 0.0F)
    {
        s *= 0.2F;
    }
    glm::vec3 sunMoonDir = glm::vec3(-r * 0.4F, r * s, r * c);
    //glm::vec3 sunMoonDir = { 100, 100, 100 };

    mShadowCam.lookAt(sunMoonDir + cam.getPosition(), cam.getPosition());
    if (mDayOptions.mViewFromLight)
    {
        cam.lookAt(glm::normalize(sunMoonDir) * 10.0f, { 0.0f, 0.0f, 0.0f });
    }

    glm::vec4 sunPosScreenSpace = cam.getProjection() * cam.getView() * glm::vec4(sunMoonDir, 1.0);
    sunPosScreenSpace = sunPosScreenSpace / sunPosScreenSpace.w;
    sunPosScreenSpace = sunPosScreenSpace * 0.5f;
    sunPosScreenSpace = sunPosScreenSpace + 0.5f;
    mSunPos = sunPosScreenSpace;
    mDayOptions.mFlarePos.x = sunPosScreenSpace.x;
    mDayOptions.mFlarePos.y = 1.0f - sunPosScreenSpace.y;

    // LampBuffer lampBuf{};
    // lampBuf.mLightPos = glm::vec4(mSunMoonDir., 1000.0f); // w is radius
    // lampBuf.mLightColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f); // w is intensity
    // lampBuf.mSpecColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f); // w is unused

    // mLampBuffer->setBufferData(&lampBuf, sizeof(LampBuffer), sizeof(LampBuffer));

    sunMoonDir = glm::normalize(glm::vec3(cam.getPosition()) - sunMoonDir);
    mSceneSettings.mSkyInfo.mSunMoonDir = glm::vec4(sunMoonDir, 1.0f);
}