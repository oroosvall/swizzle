#ifndef SCENE_HPP
#define SCENE_HPP

/* Include files */

#include <common/Common.hpp>
#include <swizzle/gfx/Context.hpp>

#include <string>
#include <fstream>

#include <assetManager/AssetManager.hpp>
#include "Renderable.hpp"
#include "Compositor.hpp"

#include "../Camera.hpp"

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
    Scene(common::Resource<AssetManager> assetManager, common::Resource<Compositor> compositor);
    ~Scene();

    void clear();

    SwBool loadScene(std::string file);

    void loadSky();
    void loadAnimMesh();

    SceneState update(DeltaTime dt, common::Resource<swizzle::gfx::CommandBuffer> cmdBuf);
    void render(common::Resource<swizzle::gfx::CommandBuffer> cmdBuf, PerspectiveCamera& cam);

private:

    SwBool loadAscii(std::ifstream& stream);
    SwBool loadBinary(std::ifstream& stream);

    common::Resource<AssetManager> mAssetManager;
    common::Resource<Compositor> mCompositor;
    SceneState mSceneState;

    std::vector<common::Resource<Renderable>> mRenderables;

};

/* Function Declaration */

#endif
