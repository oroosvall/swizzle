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
    void loadCube();
    void loadAnimTexture();
    void loadHeightMap();
    void loadTesselationMesh();

    SceneState update(DeltaTime dt, SceneRenderSettings& settings,
                      common::Unique<swizzle::gfx::CommandTransaction>& trans);
    void render(common::Unique<swizzle::gfx::DrawCommandTransaction>& trans, PerspectiveCamera& cam);

private:
    SwBool loadAscii(std::ifstream& stream);

    common::Resource<swizzle::gfx::GfxContext> mContext;
    common::Resource<Compositor> mCompositor;
    common::Resource<AssetManager> mAssetManager;
    SceneState mSceneState;

    std::vector<common::Resource<Renderable>> mRenderables;
};

/* Function Declaration */

#endif
