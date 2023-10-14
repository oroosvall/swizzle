
#pragma once

/* Includes */

#include "ShaderSources.hpp"

#include <swizzle/core/File.hpp>
#include <swizzle/gfx/Shader.hpp>

#include <unordered_map>
#include <vector>

/* Defines */

/* Typedefs */

/* Function Declaration */

/* Forward Declared Structs/Classes */

/* Struct Declaration */

/* Class Declaration */

namespace sb
{
    class InputSources
    {
    public:
        InputSources(swizzle::gfx::ShaderType shaderType);
        ~InputSources() = default;

        void addSource(swizzle::gfx::StageType stage, common::Resource<ShaderSource> source);
        swizzle::gfx::ShaderType getShaderType() const;
        common::Resource<ShaderSource> getStageSource(swizzle::gfx::StageType stage) const;

        const std::unordered_map<swizzle::gfx::StageType, common::Resource<ShaderSource>>& foreach();

    private:
        swizzle::gfx::ShaderType mShaderType;
        std::unordered_map<swizzle::gfx::StageType, common::Resource<ShaderSource>> mStageInputSource;

        const common::Resource<EmptySource> mEmptySource;
    };

    class OutputAssemblies
    {
    public:
        OutputAssemblies(swizzle::gfx::ShaderType shaderType);
        OutputAssemblies(common::Resource<swizzle::core::IFile> cacheFile);
        ~OutputAssemblies() = default;

        void setError(std::string errorStr);
        SwBool hasError() const;
        const std::string& getErrorStr() const;

        void addStage(swizzle::gfx::StageType stage, const std::vector<U32>& code);

        swizzle::gfx::ShaderType getShaderType() const;
        const std::vector<U32>& getStageCode(swizzle::gfx::StageType stage) const;

        SwBool save(common::Resource<swizzle::core::IFile> cacheFile);

    private:
        swizzle::gfx::ShaderType mShaderType;
        std::unordered_map<swizzle::gfx::StageType, std::vector<U32>> mShaderStages;
        const std::vector<U32> mEmptyCode;

        SwBool mHasError;
        std::string mErrorString;
    };

    OutputAssemblies compile(InputSources input);
} // namespace sb
