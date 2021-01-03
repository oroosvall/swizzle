#ifndef MESH_LOADER_HPP
#define MESH_LOADER_HPP

#include <swizzle/Api.hpp>
#include <swizzle/gfx/Buffer.hpp>
#include <swizzle/Resource.hpp>


namespace meshloader
{
	
	void SWIZZLE_API loadObjMeshIntoBuffer(const char* fileName, swizzle::Resource<swizzle::Buffer>& vertexBuffer);

}

#endif