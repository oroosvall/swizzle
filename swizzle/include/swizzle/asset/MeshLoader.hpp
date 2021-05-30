#ifndef MESH_LOADER_HPP
#define MESH_LOADER_HPP

/* Include files */

#include <common/Common.hpp>

#include <swizzle/Api.hpp>
#include <swizzle/gfx/Buffer.hpp>
#include <swizzle/gfx/Context.hpp>

/* Defines */

/* Typedefs/enums */

/* Forward Declared Structs/Classes */

/* Struct Declaration */

namespace swizzle
{
	struct Mesh
	{
		common::Resource<swizzle::gfx::Buffer> mVertexBuffer;
		common::Resource<swizzle::gfx::Buffer> mIndexBuffer;
	};
}

/* Class Declaration */

/* Function Declaration */

namespace swizzle::asset
{

	swizzle::Mesh SWIZZLE_API LoadMesh(common::Resource<gfx::GfxContext> ctx, const SwChar* fileName, SwBool createIndexBuffer);

}

#endif