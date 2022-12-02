
#include "Game.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <swizzle/core/Input.hpp>

#include <swizzle/asset/TextureLoader.hpp>

#include <utils/StringUtils.hpp>

#include <imgui/imgui.h>
#include <imgui/imgui_impl_swizzle.hpp>

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

    cam.setPosition({0.0F, 0.0F, 5.5F});

    sw::gfx::ShaderAttributeList attribFsq = {};
    attribFsq.mDescriptors = {
        {sw::gfx::DescriptorType::TextureSampler, sw::gfx::Count(1u), {sw::gfx::StageType::fragmentStage}},
    };
    attribFsq.mPushConstantSize = 0u;
    attribFsq.mEnableBlending = true;
    attribFsq.mPrimitiveType = sw::gfx::PrimitiveType::triangle;

    mFsq = mGfxContext->createShader(mSwapchain, sw::gfx::ShaderType::ShaderType_Graphics, attribFsq);
    mFsq->load("shaders/fsq.shader");

    mFsqMat = mGfxContext->createMaterial(mFsq, swizzle::gfx::SamplerMode::SamplerModeClamp);
    ImGui_ImplSwizzle_SetMaterial(mFsqMat);

    mCompositor = common::CreateRef<Compositor>(mGfxContext, mSwapchain);
    mScene = common::CreateRef<Scene>(mGfxContext, mCompositor);

    mScene->loadSky();
    // mScene->loadAnimMesh();
    mScene->loadCube();
    mScene->loadAnimTexture();
}

SwBool Game::userUpdate(F32 dt)
{
    OPTICK_EVENT("userUpdate");

    mFpsCounter.tick(dt);

    mInputLocked = ImGui::GetIO().WantCaptureMouse || ImGui::GetIO().WantCaptureKeyboard;

    ImGui_ImplSwizzle_NewFrame(mWindow);
    ImGui::NewFrame();

    ImGui::Begin("Statistics");
    {
        OPTICK_EVENT("ImGui::Text");
        std::string text = getStatisticsText();
        ImGui::Text("%s", text.c_str());
    }

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

    ImGui::End();

    ImGui::EndFrame();

    updateMainWindow(dt);

    return mWindow->isVisible();
}

void Game::userCleanup()
{
    mScene.reset();
    mCompositor.reset();
    mCmdBuffer.reset();
    mFrameBuffer.reset();
}

void Game::updateMainWindow(F32 dt)
{
    OPTICK_EVENT("Game::updateMainWindow");
    U32 x, y;
    mWindow->getSize(x, y);

    cam.changeAspect((F32)x, (F32)y);

    if (!mInputLocked)
    {
        mController.update(dt);
    }

    mSwapchain->setClearColor({0, 0, 0, 1});

    mSwapchain->prepare();

    auto trans = mCmdBuffer->begin();

    mScene->update(dt, trans);

    imGuiRender(trans);

    auto dTrans = mCmdBuffer->beginRenderPass(mSwapchain, std::move(trans));

    mScene->render(dTrans, cam);

    dTrans->bindShader(mFsq);
    dTrans->bindMaterial(mFsq, mFsqMat);
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

std::string Game::getStatisticsText() const
{
    std::string statisticsString;
    statisticsString.reserve(1024);

    statisticsString = "Frames: " + std::to_string(mSwapchain->getFrameCounter()) + "\n";
    statisticsString += "FPS: " + std::to_string(mFpsCounter.getFps()) + "\n\n";

    statisticsString += std::string(mGfxContext->getSelectedDeviceName()) + "\n";

    auto iter = mGfxContext->getStatisticsIterator();

    do
    {
        OPTICK_EVENT("Parse Stats");
        if (iter->getType() == sw::gfx::GfxStatsType::MemoryStats)
        {
            sw::gfx::MemoryStatistics* memStat = (sw::gfx::MemoryStatistics*)iter->getStatisticsData();

            statisticsString += "Memory Heap: " + std::string(memStat->mName) + "\n";
            statisticsString +=
                "  Mem: " + utils::toMemoryString(memStat->mUsed) + "/" + utils::toMemoryString(memStat->mSize);
            statisticsString += "; Allocs: " + std::to_string(memStat->mNumAllocations) + "p, " +
                                std::to_string(memStat->mNumVirtualAllocations) + "v\n";
        }
        else if (iter->getType() == sw::gfx::GfxStatsType::DeviceStats)
        {
            sw::gfx::DeviceStatistics* devStats = (sw::gfx::DeviceStatistics*)iter->getStatisticsData();
            statisticsString += "Device\n";
            statisticsString += "  Num Staged Objects: " + std::to_string(devStats->mNumStagedObjects) + "\n";
            statisticsString += "  Num Textures: " + std::to_string(devStats->mNumTextures) + "\n";
            statisticsString += "  Num Buffers: " + std::to_string(devStats->mNumBuffers) + "\n";
            statisticsString += "  Pipelines: " + std::to_string(devStats->mNumPipelines) + "\n";
        }
        else if (iter->getType() == sw::gfx::GfxStatsType::InstanceStats)
        {
            sw::gfx::InstanceStatistics* instStats = (sw::gfx::InstanceStatistics*)iter->getStatisticsData();
            statisticsString += "Instance\n";
            statisticsString += "  Alloc Count " + std::to_string(instStats->mAllocCount) + "\n";
            statisticsString += "  Internal Alloc Count " + std::to_string(instStats->mInternalAllocCount) + "\n";
        }
        else if (iter->getType() == sw::gfx::GfxStatsType::GfxPipelineStats)
        {
            sw::gfx::GfxPipelineStatistics* gfxStats = (sw::gfx::GfxPipelineStatistics*)iter->getStatisticsData();
            statisticsString += "Graphics Pipeline statistics\n";
            statisticsString += "  Input Assemby Vertices " + std::to_string(gfxStats->mInputAssemblyVertices) + "\n";
            statisticsString +=
                "  Input Assemby Primitives " + std::to_string(gfxStats->mInputAssemblyPrimitives) + "\n";
            statisticsString +=
                "  Vertex Shader Invocations " + std::to_string(gfxStats->mVertexShaderInvocations) + "\n";
            statisticsString += "  Clipping Invocations " + std::to_string(gfxStats->mClippingInvocations) + "\n";
            statisticsString += "  Clipping Primitives " + std::to_string(gfxStats->mClippingInvocations) + "\n";
            statisticsString +=
                "  Fragment Shader Invocations " + std::to_string(gfxStats->mFragmentShaderInvocations) + "\n";
            statisticsString += "  Tesselation Control Shader Patches " +
                                std::to_string(gfxStats->mTesselationControlShaderPatches) + "\n";
            statisticsString += "  Tesselation Evaluation Shader Invocations " +
                                std::to_string(gfxStats->mTesselationEvaluationShaderInvocations) + "\n";
            statisticsString +=
                "  Compute Shader Invocations " + std::to_string(gfxStats->mComputeShaderInvocations) + "\n";
        }

    } while (iter->next());

    statisticsString += "Draw call count: " + std::to_string(mCmdBuffer->getDrawCount()) + "\n";
    statisticsString += "Vertex count: " + std::to_string(mCmdBuffer->getVertCount()) + "\n";
    statisticsString += "Triangle count: " + std::to_string(mCmdBuffer->getTriCount()) + "\n";

    return statisticsString;
}