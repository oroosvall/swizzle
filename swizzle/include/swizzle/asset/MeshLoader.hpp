#ifndef MESH_LOADER_HPP
#define MESH_LOADER_HPP

/* Include files */

#include <common/Types.hpp>

#include <swizzle/Api.hpp>
#include <swizzle/gfx/Buffer.hpp>
#include <swizzle/gfx/Context.hpp>
#include <common/Resource.hpp>

/* Defines */

/* Typedefs/enums */

/* Forward Declared Structs/Classes */

/* Struct Declaration */

namespace swizzle
{
	struct Mesh
	{
		swizzle::core::Resource<swizzle::gfx::Buffer> mVertexBuffer;
		swizzle::core::Resource<swizzle::gfx::Buffer> mIndexBuffer;
	};
}

/* Class Declaration */

/* Function Declaration */

namespace swizzle::asset
{

	swizzle::Mesh SWIZZLE_API LoadMesh(core::Resource<gfx::GfxContext> ctx, const SwCharPtr fileName, SwBool createIndexBuffer);

}

#endif