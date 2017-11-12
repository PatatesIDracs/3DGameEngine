#include "ResourceMesh.h"
#include "Glew\include\glew.h"

RenderData::~RenderData()
{
	if (num_indices > 0)		glDeleteBuffers(1, &id_indices);
	if (num_vertices > 0)		glDeleteBuffers(1, &id_vertices);
	if (num_normals > 0)		glDeleteBuffers(1, &id_normals);
	if (num_tex_vertices > 0)	glDeleteBuffers(1, &id_tex_vertices);
	
	delete[] indices;
	delete[] vertices;
	delete[] normals;
	delete[] tex_vertices;
}

ResourceMesh::ResourceMesh(int uid) : Resource(uid)
{
	type = RESOURCE_TYPE::RESOURCE_MESH;

	aabb_box.SetNegativeInfinity();
}

ResourceMesh::~ResourceMesh()
{
}

const RenderData * ResourceMesh::GetRenderData() const
{
	return render_data;
}

AABB ResourceMesh::GetAABB() const
{
	return aabb_box;
}
