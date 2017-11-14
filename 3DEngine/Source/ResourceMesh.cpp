#include "ResourceMesh.h"
#include "Globals.h"
#include "Glew\include\glew.h"

#include <fstream>

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


void ResourceMesh::SaveResource()
{
	//Set names 
	library_file = std::to_string(uid);
	library_file.append(".mjope");

	uint buffer_size = 0;
	GetBufferSize(buffer_size);
	uint ranges[4] = { render_data->num_indices, render_data->num_vertices,render_data->num_tex_vertices,  render_data->num_normals };


	char* buffer_data = new char[buffer_size];
	char* cursor = buffer_data;

	//Resource Copy
	uint bytes_to_copy = sizeof(RESOURCE_TYPE);
	memcpy(cursor, &type, bytes_to_copy);
	cursor += bytes_to_copy; //Advance cursor

	bytes_to_copy = sizeof(int);
	memcpy(cursor, &uid, bytes_to_copy);
	cursor += bytes_to_copy; //Advance cursor

	bytes_to_copy = sizeof(name.c_str());
	memcpy(cursor, &uid, bytes_to_copy);
	cursor += bytes_to_copy; //Advance cursor

	bytes_to_copy = sizeof(assets_file.c_str());
	memcpy(cursor, &uid, bytes_to_copy);
	cursor += bytes_to_copy; //Advance cursor

	bytes_to_copy = sizeof(library_file.c_str());
	memcpy(cursor, &uid, bytes_to_copy);
	cursor += bytes_to_copy; //Advance cursor



	bytes_to_copy = sizeof(ranges);
	memcpy(cursor, ranges, bytes_to_copy);
	cursor += bytes_to_copy; //Advance cursor

	bytes_to_copy = (sizeof(uint) * render_data->num_indices);
	memcpy(cursor, render_data->indices, bytes_to_copy);
	cursor += bytes_to_copy; //Advance cursor

	bytes_to_copy = (sizeof(float) * render_data->num_vertices * 3);
	memcpy(cursor, render_data->vertices, bytes_to_copy);
	cursor += bytes_to_copy; //Advance cursor

	bytes_to_copy = (sizeof(float) * render_data->num_tex_vertices * 3);
	memcpy(cursor, render_data->tex_vertices, bytes_to_copy);
	cursor += bytes_to_copy; //Advance cursor

	bytes_to_copy = (sizeof(float) * render_data->num_normals * 3);
	memcpy(cursor, render_data->normals, bytes_to_copy);

	std::string save_path = JOPE_DATA_DIRECTORY JOPE_LIBRARY_FOLDER JOPE_MESHES_FOLDER;
	std::ofstream new_file((save_path + library_file).c_str(), std::ofstream::binary);
	new_file.write(buffer_data, buffer_size);

	LOGC("File Saved at: %s", library_file.c_str());

//	delete buffer_data;

}

void ResourceMesh::GetBufferSize(uint & buffer_size)
{
	Resource::GetBufferSize(buffer_size);

	uint ranges[4] = { render_data->num_indices, render_data->num_vertices,render_data->num_tex_vertices,  render_data->num_normals };
	buffer_size = sizeof(ranges) +
		(sizeof(uint) * render_data->num_indices) +				//Indices
		(sizeof(float) * render_data->num_vertices * 3) +		//Vertices
		(sizeof(float) * render_data->num_tex_vertices * 3) +	//Texture_coords
		(sizeof(float) * render_data->num_normals * 3);			//Normals
}
