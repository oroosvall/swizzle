#ifndef MESH_LOADER_HPP
#define MESH_LOADER_HPP

#include <swizzle/Api.hpp>
#include <swizzle/gfx/Buffer.hpp>
#include <common/Resource.hpp>


namespace meshloader
{

	void SWIZZLE_API loadObjMeshIntoBuffer(const char* fileName, swizzle::core::Resource<swizzle::gfx::Buffer>& vertexBuffer);

}

#endif