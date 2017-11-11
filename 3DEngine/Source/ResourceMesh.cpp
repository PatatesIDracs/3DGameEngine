#include "ResourceMesh.h"

ResourceMesh::ResourceMesh(int uid) : Resource(uid)
{
	type = RESOURCE_TYPE::RESOURCE_MESH;
}

ResourceMesh::~ResourceMesh()
{
}
