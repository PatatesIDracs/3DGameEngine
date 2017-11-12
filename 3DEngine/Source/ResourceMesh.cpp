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
	delete render_data;
}

void ResourceMesh::SetRenderData(RenderData * new_render_data)
{
	if (render_data != nullptr)
		delete render_data;
	render_data = new_render_data;
	aabb_box.Enclose((float3*)render_data->vertices, render_data->num_vertices);
}

void ResourceMesh::LoadToMemory()
{
	// Load Vertices and Indices To Buffer and Set ID
	if (render_data->vertices != nullptr)
	{
		glGenBuffers(1, (GLuint*)&render_data->id_vertices);
		glBindBuffer(GL_ARRAY_BUFFER, render_data->id_vertices);
		glBufferData(GL_ARRAY_BUFFER, render_data->num_vertices * 3 * sizeof(float), &render_data->vertices[0], GL_STATIC_DRAW);
	}

	if (render_data->normals != nullptr)
	{
		glGenBuffers(1, (GLuint*)&render_data->id_normals);
		glBindBuffer(GL_ARRAY_BUFFER, render_data->id_normals);
		glBufferData(GL_ARRAY_BUFFER, render_data->num_normals * 3 * sizeof(float), &render_data->normals[0], GL_STATIC_DRAW);
	}

	if (render_data->indices != nullptr)
	{
		glGenBuffers(1, (GLuint*)&render_data->id_indices);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, render_data->id_indices);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, render_data->num_indices * sizeof(uint), &render_data->indices[0], GL_STATIC_DRAW);
	}

	// Load texture coords buffer
	if (render_data->tex_vertices != nullptr)
	{
		glGenBuffers(1, (GLuint*)&render_data->id_tex_vertices);
		glBindBuffer(GL_ARRAY_BUFFER, render_data->id_tex_vertices);
		glBufferData(GL_ARRAY_BUFFER, render_data->num_tex_vertices * 3 * sizeof(float), &render_data->tex_vertices[0], GL_STATIC_DRAW);
	}
}

const RenderData * ResourceMesh::GetRenderData() const
{
	return render_data;
}

AABB ResourceMesh::GetAABB() const
{
	return aabb_box;
}
