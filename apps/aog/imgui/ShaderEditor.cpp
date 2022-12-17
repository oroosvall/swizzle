
/* Include files */

#include "ShaderEditor.hpp"

#include <fstream>
#include <sstream>

#include <filesystem>

/* Defines */

/* Typedefs */

/* Structs/Classes */

/* Static Variables */

/* Static Function Declaration */

static void shaderInfoSplitEquals(const std::string& info, std::string& type, std::string& path)
{
    auto index = info.find('=');
    type = info.substr(0, index);
    path = info.substr(index + 1);
}

static std::string pathFromFilename(const std::string& fileName)
{
    std::size_t found = fileName.find_last_of("/\\");
    return fileName.substr(0, found + 1);
}

static std::string filenameFromPath(const std::string& fileName)
{
    std::size_t found = fileName.find_last_of("/\\");
    return fileName.substr(found + 1);
}

/* Static Function Definition */

struct InputTextCallback_UserData
{
    std::string* Str;
    ImGuiInputTextCallback ChainCallback;
    void* ChainCallbackUserData;
};

static int InputTextCallback(ImGuiInputTextCallbackData* data)
{
    InputTextCallback_UserData* user_data = (InputTextCallback_UserData*)data->UserData;
    if (data->EventFlag == ImGuiInputTextFlags_CallbackResize)
    {
        // Resize string callback
        // If for some reason we refuse the new length (BufTextLen) and/or capacity (BufSize) we need to set them back
        // to what we want.
        std::string* str = user_data->Str;
        IM_ASSERT(data->Buf == str->c_str());
        str->resize(data->BufTextLen);
        data->Buf = (char*)str->c_str();
    }
    else if (user_data->ChainCallback)
    {
        // Forward to user callback, if any
        data->UserData = user_data->ChainCallbackUserData;
        return user_data->ChainCallback(data);
    }
    return 0;
}

bool InputTextMultiline(const char* label, std::string* str, const ImVec2& size = ImVec2(0, 0),
                        ImGuiInputTextFlags flags = 0, ImGuiInputTextCallback callback = NULL, void* user_data = NULL)
{
    IM_ASSERT((flags & ImGuiInputTextFlags_CallbackResize) == 0);
    flags |= ImGuiInputTextFlags_CallbackResize;

    InputTextCallback_UserData cb_user_data{};
    cb_user_data.Str = str;
    cb_user_data.ChainCallback = callback;
    cb_user_data.ChainCallbackUserData = user_data;
    return ImGui::InputTextMultiline(label, (char*)str->c_str(), str->capacity() + 1, size, flags, InputTextCallback,
                                     &cb_user_data);
}

/* Function Definition */

/* Class Public Function Definition */

ShaderEditor::ShaderEditor(common::Resource<AssetManager> assetManager)
    : mAssetManager(assetManager)
    , mIndex(0)
    , mDebugSymbols(false)
{
}

void ShaderEditor::render()
{
    ImGui::Begin("Shader Editor", 0, ImGuiWindowFlags_::ImGuiWindowFlags_MenuBar);
    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("Save", "Ctrl - S", false) ||
                (ImGui::GetIO().KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_::ImGuiKey_S)))
            {
                saveCurrentFile();
            }
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }

    auto info = mAssetManager->getShaderInfo(mIndex);
    ImGui::Text("Shader files:");
    ImGui::SameLine();
    if (ImGui::BeginCombo("##shader", info.mFilePath))
    {
        for (size_t i = 0ull; i < mAssetManager->getShaderCount(); ++i)
        {
            auto iterItem = mAssetManager->getShaderInfo(i);
            std::string text = { iterItem.mFilePath };
            text += "##" + std::to_string(i);
            if (ImGui::Selectable(iterItem.mFilePath, mIndex == i))
            {
                mIndex = (U32)i;
            }
        }

        ImGui::EndCombo();
    }
    info = mAssetManager->getShaderInfo(mIndex);
    auto& selectedCache = getShader(info.mFilePath);

    ImGui::Text("Shader:");
    ImGui::SameLine();
    if (ImGui::BeginCombo("##shaderTypes", selectedCache.mSelectedKey.c_str()))
    {
        for (auto& it : selectedCache.mTexts)
        {
            auto& str = it.first;
            if (ImGui::Selectable(str.c_str(), str == selectedCache.mSelectedKey))
            {
                selectedCache.mSelectedKey = it.first;
            }
        }

        ImGui::EndCombo();
    }

    selectedCache = getShader(info.mFilePath);

    std::string* text = &selectedCache.mTexts[selectedCache.mSelectedKey];

    InputTextMultiline("##shaderInput", text, ImVec2(800, 600));
    ImGui::Checkbox("Debug symbols", &mDebugSymbols);
    if (ImGui::Button("Compile Shader"))
    {
        saveCurrentFile();
        compileShader(selectedCache);
        mAssetManager->reloadShader(mIndex);
    }

    ImGui::End();
}

/* Class Protected Function Definition */

/* Class Private Function Definition */

void ShaderEditor::saveCurrentFile()
{
    auto info = mAssetManager->getShaderInfo(mIndex);
    auto& selectedCache = getShader(info.mFilePath);

    std::ofstream outFile(selectedCache.mSelectedKey);
    outFile << selectedCache.mTexts[selectedCache.mSelectedKey];
}

ShaderTextCache& ShaderEditor::getShader(const SwChar* file)
{
    if (mCache.count(file) == 0)
    {
        ShaderTextCache cache;

        std::ifstream shaderFile(file);
        if (shaderFile.is_open())
        {
            std::string line = "";
            bool src = false;
            bool firstRead = false;

            while (true)
            {
                std::getline(shaderFile, line);
                if (shaderFile.eof())
                {
                    break;
                }

                if (line == "[src]")
                {
                    src = true;
                    continue;
                }
                else if (line[0] == '[')
                {
                    src = false;
                }
                else
                {
                    // Do Nothing
                }

                if (src)
                {
                    std::string shaderType = "";
                    std::string path = "";

                    shaderInfoSplitEquals(line, shaderType, path);
                    path.insert(0, pathFromFilename(file));

                    std::ifstream content(path);
                    std::stringstream buffer;
                    buffer << content.rdbuf();

                    cache.mTexts[path] = buffer.str();
                    if (!firstRead)
                    {
                        cache.mSelectedKey = path;
                        firstRead = true;
                    }
                }
            }
        }
        mCache[file] = cache;
    }

    return mCache[file];
}

void ShaderEditor::compileShader(ShaderTextCache& cache)
{
    std::string selectedPath = cache.mSelectedKey;
    std::string outDir = pathFromFilename(selectedPath);
    std::string fileName = filenameFromPath(selectedPath);

    auto path = std::filesystem::current_path();
    std::filesystem::current_path(outDir);
    std::string cmd = "glslangvalidator -V --target-env spirv1.6 ";
    if (mDebugSymbols)
    {
        cmd += "-g ";
    }
    cmd += fileName;
    system(cmd.c_str());
    std::filesystem::current_path(path);
}