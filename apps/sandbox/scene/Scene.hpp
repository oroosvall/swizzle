#ifndef SCENE_HPP
#define SCENE_HPP

/* Include files */

#include <common/Common.hpp>
#include <swizzle/gfx/Context.hpp>

#include <fstream>
#include <string>
#include <functional>

#include "Compositor.hpp"
#include "Renderable.hpp"
#include <assetManager/AssetManager.hpp>

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
    Scene(common::Resource<swizzle::gfx::GfxDevice> dev, common::Resource<AssetManager> assetManager,
          common::Resource<Compositor> compositor);
    ~Scene();

    void clear();

    SwBool loadScene(std::string file);

    void loadSky();
    void loadAnimMesh();

    SceneState update(DeltaTime dt, common::Unique<swizzle::gfx::CommandTransaction>& trans);
    void render(common::Unique<swizzle::gfx::DrawCommandTransaction>& trans, PerspectiveCamera& cam);

private:
    SwBool loadAscii(std::ifstream& stream);
    SwBool loadBinary(std::ifstream& stream);

    common::Resource<swizzle::gfx::GfxDevice> mDevice;
    common::Resource<AssetManager> mAssetManager;
    common::Resource<Compositor> mCompositor;
    SceneState mSceneState;

    std::vector<common::Resource<Renderable>> mRenderables;


    std::vector<std::function<void(common::Unique<swizzle::gfx::CommandTransaction>& trans)>> mTextureUpload;

};

/* Function Declaration */

#endif
