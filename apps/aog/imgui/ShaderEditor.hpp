#ifndef SHADER_EDITOR_HPP
#define SHADER_EDITOR_HPP

/* Include files */

#include <imgui/imgui.h>
#include "../AssetManager.hpp"

#include <string>
#include <map>

/* Defines */

/* Typedefs/enums */

/* Forward Declared Structs/Classes */

/* Struct Declaration */

struct ShaderTextCache
{
    std::string mSelectedKey;
    std::map<std::string, std::string> mTexts;
};

/* Class Declaration */

class ShaderEditor
{
public:
    ShaderEditor(common::Resource<AssetManager> assetManager);

    void render();

private:

    void saveCurrentFile();
    common::Resource<AssetManager> mAssetManager;

    ShaderTextCache& getShader(const SwChar* file);
    void compileShader(ShaderTextCache& cache);

    uint32_t mIndex;
    bool mDebugSymbols;
    std::map<const SwChar*, ShaderTextCache> mCache;

};

/* Function Declaration */

#endif
