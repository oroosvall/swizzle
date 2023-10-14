
/* Include files */

#include <shader/ShaderBuilder.hpp>
#include <shaderc/shaderc.hpp>

#include <random>

/* Defines */

/* Typedefs */

/* Structs/Classes */

/* Static Variables */

/* Static Function Declaration */

shaderc_shader_kind ToShadercKind(swizzle::gfx::StageType type);
std::string GetTypeName(swizzle::gfx::StageType type);
std::string RandomString(std::size_t lenght);
std::string CompilationStatusString(shaderc_compilation_status status);

/* Static Function Definition */

shaderc_shader_kind ToShadercKind(swizzle::gfx::StageType type)
{
    switch (type)
    {
    case swizzle::gfx::StageType::vertexStage:
        return shaderc_shader_kind::shaderc_vertex_shader;
    case swizzle::gfx::StageType::tessellationStage:
        return shaderc_shader_kind::shaderc_tess_control_shader;
    case swizzle::gfx::StageType::meshStage:
        return shaderc_shader_kind::shaderc_mesh_shader;
    case swizzle::gfx::StageType::taskStage:
        return shaderc_shader_kind::shaderc_task_shader;
    case swizzle::gfx::StageType::fragmentStage:
        return shaderc_shader_kind::shaderc_fragment_shader;
    case swizzle::gfx::StageType::geometryStage:
        return shaderc_shader_kind::shaderc_geometry_shader;
    case swizzle::gfx::StageType::computeStage:
        return shaderc_shader_kind::shaderc_compute_shader;
    default:
        return shaderc_shader_kind::shaderc_glsl_default_vertex_shader;
        break;
    }
}

std::string GetTypeName(swizzle::gfx::StageType type)
{
    switch (type)
    {
    case swizzle::gfx::StageType::vertexStage:
        return "vertex";
    case swizzle::gfx::StageType::tessellationStage:
        return "tesselation";
    case swizzle::gfx::StageType::meshStage:
        return "mesh";
    case swizzle::gfx::StageType::taskStage:
        return "task";
    case swizzle::gfx::StageType::fragmentStage:
        return "fragment";
    case swizzle::gfx::StageType::geometryStage:
        return "geometry";
    case swizzle::gfx::StageType::computeStage:
        return "compute";
    default:
        return "default";
        break;
    }
}

std::string RandomString(std::size_t length)
{
    const std::string CHARACTERS = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

    std::random_device randomDevice;
    std::mt19937 generator(randomDevice());
    std::uniform_int_distribution<> distribution(0, static_cast<S32>(CHARACTERS.size()) - 1);

    std::string randomStr;

    for (std::size_t i = 0ull; i < length; ++i)
    {
        randomStr += CHARACTERS[distribution(generator)];
    }

    return randomStr;
}

std::string CompilationStatusString(shaderc_compilation_status status)
{
    switch (status)
    {
    case shaderc_compilation_status_invalid_stage:
        return "invalid stage";
    case shaderc_compilation_status_compilation_error:
        return "compilation error";
    case shaderc_compilation_status_internal_error:
        return "internal error";
    case shaderc_compilation_status_null_result_object:
        return "null result";
    case shaderc_compilation_status_invalid_assembly:
        return "invalid assembly";
    case shaderc_compilation_status_validation_error:
        return "validation error";
    case shaderc_compilation_status_transformation_error:
        return "transformation error";
    case shaderc_compilation_status_configuration_error:
        return "configuration error";
    case shaderc_compilation_status_success:
    default:
        return "success";
        break;
    }
}

/* Function Definition */

namespace sb
{
    OutputAssemblies compile(InputSources input)
    {
        shaderc::Compiler comp{};
        shaderc::CompileOptions options{};
        // options.SetGenerateDebugInfo();

        OutputAssemblies output{input.getShaderType()};

        std::string moduleName = RandomString(6);

        for (const auto& it : input.foreach ())
        {
            U32 size = 0;
            const char* src = it.second->getSource(size);
            shaderc_shader_kind kind = ToShadercKind(it.first);
            std::string name = moduleName + GetTypeName(it.first);
            shaderc::SpvCompilationResult result = comp.CompileGlslToSpv(src, size, kind, name.c_str(), options);
            if (result.GetCompilationStatus() == shaderc_compilation_status::shaderc_compilation_status_success)
            {
                std::vector<U32> code{result.begin(), result.end()};
                output.addStage(it.first, code);
            }
            else
            {
                std::string errorStr = CompilationStatusString(result.GetCompilationStatus());
                errorStr += "\n";
                errorStr += result.GetErrorMessage();
                output.setError(errorStr);
                break;
            }
        }

        return output;
    }
} // namespace sb

/* Class Public Function Definition */

namespace sb
{
    InputSources::InputSources(swizzle::gfx::ShaderType shaderType)
        : mShaderType(shaderType)
        , mStageInputSource()
        , mEmptySource()
    {
    }

    void InputSources::addSource(swizzle::gfx::StageType stage, common::Resource<ShaderSource> source)
    {
        mStageInputSource[stage] = source;
    }

    swizzle::gfx::ShaderType InputSources::getShaderType() const
    {
        return mShaderType;
    }

    common::Resource<ShaderSource> InputSources::getStageSource(swizzle::gfx::StageType stage) const
    {
        auto s = mStageInputSource.find(stage);
        if (s != mStageInputSource.end())
        {
            return s->second;
        }
        return mEmptySource;
    }

    const std::unordered_map<swizzle::gfx::StageType, common::Resource<ShaderSource>>& InputSources::foreach ()
    {
        return mStageInputSource;
    }

    OutputAssemblies::OutputAssemblies(swizzle::gfx::ShaderType shaderType)
        : mShaderType(shaderType)
        , mShaderStages()
        , mEmptyCode()
        , mHasError(false)
        , mErrorString("")
    {
    }

    OutputAssemblies::OutputAssemblies(common::Resource<swizzle::core::IFile> cacheFile)
        : mShaderType()
        , mShaderStages()
        , mEmptyCode()
        , mHasError(false)
        , mErrorString("")
    {
        UNUSED_ARG(cacheFile);
        // TODO
    }

    void OutputAssemblies::setError(std::string errorStr)
    {
        mHasError = true;
        mErrorString = errorStr;
    }

    SwBool OutputAssemblies::hasError() const
    {
        return mHasError;
    }

    const std::string& OutputAssemblies::getErrorStr() const
    {
        return mErrorString;
    }

    void OutputAssemblies::addStage(swizzle::gfx::StageType stage, const std::vector<U32>& code)
    {
        mShaderStages[stage] = code;
    }

    swizzle::gfx::ShaderType OutputAssemblies::getShaderType() const
    {
        return mShaderType;
    }

    const std::vector<U32>& OutputAssemblies::getStageCode(swizzle::gfx::StageType stage) const
    {
        auto s = mShaderStages.find(stage);
        if (s != mShaderStages.end())
        {
            return s->second;
        }
        return mEmptyCode;
    }

    SwBool OutputAssemblies::save(common::Resource<swizzle::core::IFile> cacheFile)
    {
        UNUSED_ARG(cacheFile);
        if (!mHasError)
        {
            // TODO
        }
        return false;
    }
} // namespace sb

/* Class Protected Function Definition */

/* Class Private Function Definition */