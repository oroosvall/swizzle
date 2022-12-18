
#include "DayOptions.hpp"
#include <imgui/imgui.h>
#include <scene/SceneSettings.hpp>

#pragma warning(push)
#pragma warning(disable : 4201)
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/type_ptr.hpp>
#pragma warning(pop)

void DrawDayOptions(SceneRenderSettings& settings, DayOptions& opt)
{
    ImGui::Text("(Day 1) Enable normalmaps:");
    ImGui::SameLine();
    ImGui::Checkbox("##day1", &settings.mEnableNormalMaps);
    ImGui::Text("(Day 2) Animated Textures:");
    ImGui::SameLine();
    ImGui::Checkbox("##day2", &settings.mEnableAnimatedTextures);
    ImGui::Text("(Day 3) Show Shader Editor:");
    ImGui::SameLine();
    ImGui::Checkbox("##day3", &opt.mShowShaderEditor);
    ImGui::Text("(Day 4) Heightmap (cpu):");
    ImGui::SameLine();
    ImGui::Checkbox("##day4", &settings.mHeightMap);
    ImGui::Text("(Day 5) Tesselation:");
    ImGui::SameLine();
    ImGui::Checkbox("##day5", &settings.mTesselation);
    ImGui::Text("(Day 6) Depth of field:");
    ImGui::SameLine();
    ImGui::Checkbox("##day6", &opt.mEnableDof);
    ImGui::SliderFloat("Focal Point", &opt.mDoFFocalPoint, 0.01f, 100.0f);
    ImGui::SliderFloat("Focal Scale", &opt.mDoFFocalScale, 0.01f, 200.0f);
    ImGui::Text("(Day 7) Particle system:");
    ImGui::SameLine();
    ImGui::Checkbox("##day7", &settings.mParticles);
    ImGui::Text("(Day 8) Glow:");
    ImGui::SameLine();
    ImGui::Checkbox("##day8", &opt.mGlow);
    ImGui::Text("(Day 9) MipMaps:");
    ImGui::SameLine();
    ImGui::Checkbox("##day9", &settings.mMipMaps);
    ImGui::Text("Force lowest mip:");
    ImGui::SameLine();
    ImGui::BeginDisabled(!settings.mMipMaps);
    ImGui::Checkbox("##day9mipflag", &settings.mForceLowestMipLayer);
    ImGui::EndDisabled();
    ImGui::BeginDisabled(!opt.mHasMeshShaderSupport);
    ImGui::Text("(Day 10) MeshShaders:");
    ImGui::SameLine();
    ImGui::Checkbox("##day10", &settings.mMeshShaders);
    ImGui::EndDisabled();
    ImGui::Text("(Day 11) Bezier curves:");
    ImGui::SameLine();
    ImGui::Checkbox("##day11", &opt.mBesierCurves);
    ImGui::Text("Bezier curves editor:");
    ImGui::SameLine();
    ImGui::Checkbox("##day11edit", &opt.mBesierCurvesEditor);
    ImGui::Text("(Day 12) Camera path:");
    ImGui::SameLine();
    ImGui::Checkbox("##day12", &opt.mCameraPath);
    ImGui::Text("Camera path time:");
    ImGui::SameLine();
    ImGui::DragFloat("##day12path", &opt.mCameraTime, 0.01f, 0.0f, 1.0f);
    ImGui::Text("(Day 13) Day night + light dithering:");
    ImGui::SameLine();
    ImGui::Checkbox("##day13dithering", &opt.mDithering);
    ImGui::BeginDisabled(true);
    ImGui::Text("Sun moon dir:");
    ImGui::SameLine();
    ImGui::InputFloat3("##day13sunmoondir", glm::value_ptr(settings.mSkyInfo.mSunMoonDir));
    ImGui::Text("Time:");
    ImGui::SameLine();
    ImGui::InputFloat("##day13time", &opt.mSkyTime);
    ImGui::EndDisabled();
    ImGui::Text("Enable time:");
    ImGui::SameLine();
    ImGui::Checkbox("##day13timeEnable", &opt.mCountSkyTime);
    ImGui::Text("Day sky color:");
    ImGui::SameLine();
    ImGui::ColorEdit3("##day13dayskycolor", glm::value_ptr(settings.mSkyInfo.mDaySkyColor), ImGuiColorEditFlags_NoInputs);
    ImGui::Text("Night sky color:");
    ImGui::SameLine();
    ImGui::ColorEdit3("##day13nightskycolor", glm::value_ptr(settings.mSkyInfo.mNightSkyColor), ImGuiColorEditFlags_NoInputs);
    ImGui::Text("Sun 1 color:");
    ImGui::SameLine();
    ImGui::ColorEdit3("##day13sun1color", glm::value_ptr(settings.mSkyInfo.mSun1Color), ImGuiColorEditFlags_NoInputs);
    //ImGui::Text("Sun 2 color:");
    ImGui::SameLine();
    ImGui::ColorEdit3("##day13sun2color", glm::value_ptr(settings.mSkyInfo.mSun2Color), ImGuiColorEditFlags_NoInputs);
    ImGui::Text("Moon 1 color:");
    ImGui::SameLine();
    ImGui::ColorEdit3("##day13moon1color", glm::value_ptr(settings.mSkyInfo.mMoon1Color), ImGuiColorEditFlags_NoInputs);
    //ImGui::Text("Moon 2 color:");
    ImGui::SameLine();
    ImGui::ColorEdit3("##day13moon2color", glm::value_ptr(settings.mSkyInfo.mMoon2Color), ImGuiColorEditFlags_NoInputs);
    ImGui::Text("(Day 14) Lens flare:");
    ImGui::SameLine();
    ImGui::Checkbox("##day14lensflare", &opt.mLensFlare);
    ImGui::Text("Flare position");
    ImGui::SameLine();
    ImGui::SliderFloat2("##day14", glm::value_ptr(opt.mFlarePos), 0.0f, 1.0f);
    ImGui::Text("(Day 15) Texture viewer");
    ImGui::SameLine();
    ImGui::Checkbox("##day15", &opt.mTextureViewer);
    ImGui::Text("(Day 17) Asset streaming showcase");
    ImGui::SameLine();
    ImGui::Checkbox("##day17", &settings.mAssetSlowLoad);
}