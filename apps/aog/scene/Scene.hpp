#ifndef SCENE_HPP
#define SCENE_HPP

/* Include files */

#include <common/Common.hpp>
#include <swizzle/gfx/Context.hpp>

#include <fstream>
#include <string>

#include "../AssetManager.hpp"
#include "Compositor.hpp"
#include "Renderable.hpp"

#include "../Camera.hpp"
#include "SceneSettings.hpp"

/* Defines */

/* Typedefs/enums */

enum class SceneState
{
    NotLoaded,           // Nothing in scene, default state
    LoadingShowProgress, // Scene is waiting for assets to load, show progress indicator.
    Render,              // It is ok to render, everything needed should already be loaded.
    RenderAssetStream,   // Ok to render, asset popin might occur.
};

/* Forward Declared Structs/Classes */

/* Struct Declaration */

/* Class Declaration */

class Scene
{
public:
    Scene(common::Resource<swizzle::gfx::GfxContext> ctx, common::Resource<Compositor> compositor,
          common::Resource<AssetManager> assetManager);
    ~Scene();

    void clear();

    SwBool loadScene(std::string file);

    void loadSky();
    void loadAnimMesh();
    void loadHeightMap();
    void loadTesselationMesh();
    void loadGlow();
    void loadMeshShader();

    SceneState update(DeltaTime dt, SceneRenderSettings& settings,
                      common::Unique<swizzle::gfx::CommandTransaction>& trans);
    void render(common::Unique<swizzle::gfx::DrawCommandTransaction>& trans, PerspectiveCamera& cam);

private:
    SwBool loadAscii(std::ifstream& stream);
    common::Resource<swizzle::gfx::Buffer> createInstanceBuffer(std::vector<glm::mat4>& matrices);

    void loadThing(std::string);

    void loadParticleSystemParams(const SwChar* texturePath, glm::vec3 pos, glm::vec3 dir, U32 count);
    void loadAnimTextureParams(const SwChar* meshFile, const SwChar* texture, std::vector<glm::mat4>& inst);
    void loadRegular(const SwChar* meshFile, const SwChar* textureFile, std::vector<glm::mat4>& inst);

    common::Resource<swizzle::gfx::GfxContext> mContext;
    common::Resource<Compositor> mCompositor;
    common::Resource<AssetManager> mAssetManager;
    SceneState mSceneState;

    std::vector<common::Resource<Renderable>> mRenderables;

    std::vector<std::string> mItemsToLoad;
    F32 mSlowTime = 0.0f;
};

/* Function Declaration */

#endif
