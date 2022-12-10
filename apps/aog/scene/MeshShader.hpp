#ifndef MESH_SHADER_HPP
#define MESH_SHADER_HPP

/* Include files */

#include "Renderable.hpp"

#include <swizzle/asset2/Assets.hpp>
#include <swizzle/gfx/Context.hpp>

/* Defines */

/* Typedefs/enums */

/* Forward Declared Structs/Classes */

/* Struct Declaration */

struct MeshletDescription
{
    U32 mVertexCount;
    U32 mPrimitiveCount;
    U32 mVertexBegin;
    U32 mPrimitiveBegin;
};

/* Class Declaration */

class MeshShader : public Renderable
{
public:
    MeshShader(common::Resource<swizzle::gfx::GfxContext> ctx, common::Resource<swizzle::asset2::IMeshAsset> asset,
               common::Resource<swizzle::gfx::Buffer> inst, common::Resource<swizzle::gfx::Shader> shader, common::Resource<swizzle::gfx::Shader> normalShader);

    virtual void update(DeltaTime dt, SceneRenderSettings& settings,
                        common::Unique<swizzle::gfx::CommandTransaction>& trans) override;
    virtual void render(common::Unique<swizzle::gfx::DrawCommandTransaction>& trans, PerspectiveCamera& cam) override;

private:
    void generateMeshlets(common::Resource<swizzle::gfx::GfxContext> ctx);
    void createIndex(U32 index, std::vector<U32>& vertexList, std::vector<U32>& primitive, MeshletDescription& meshletDescr);
    SwBool isInList(U32 index, std::vector<U32>& vertexList, U32 begin, U32& outIndex);

    common::Resource<swizzle::asset2::IMeshAsset> mAsset;

    common::Resource<swizzle::gfx::Buffer> mVertexData;
    common::Resource<swizzle::gfx::Buffer> mVertexIndex;
    common::Resource<swizzle::gfx::Buffer> mPrimitiveIndex;
    common::Resource<swizzle::gfx::Buffer> mMeshlets;
    common::Resource<swizzle::gfx::Shader> mShader;
    common::Resource<swizzle::gfx::Material> mMaterial;

    common::Resource<swizzle::gfx::Buffer> mNormalVertex;
    common::Resource<swizzle::gfx::Buffer> mNormalIndex;
    common::Resource<swizzle::gfx::Shader> mNormalShader;
    common::Resource<swizzle::gfx::Material> mNormalMaterial;

    common::Resource<swizzle::gfx::Buffer> mInst;

    U32 mNumMeshlets;
    SwBool mHasMeshShader;
    SwBool mMeshShaderEnabled;
};

/* Function Declaration */

#endif
