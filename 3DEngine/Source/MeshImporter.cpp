#include "MeshImporter.h"
#include "Mesh.h"
#include "Globals.h"
#include "Glew\include\glew.h"

#include <fstream>

#include "Assimp/include/cimport.h"
#include "Assimp/include/scene.h"
#include "Assimp/include/postprocess.h"
#include "Assimp/include/cfileio.h"

#pragma comment (lib, "Assimp/libx86/assimp.lib")

MeshImporter::MeshImporter()
{
}

MeshImporter::~MeshImporter()
{
}

void MeshImporter::Import(const char* full_path)
{
	if (full_path == nullptr) return;

	std::string import_path("../Data/Library/Meshes/");

	const aiScene* scene = aiImportFile(full_path , aiProcessPreset_TargetRealtime_MaxQuality);
	aiNode* mesh_root_node = scene->mRootNode;
	if (scene != nullptr && scene->HasMeshes())
	{
		uint num_meshes = scene->mNumMeshes;
		for (uint i = 0; i < num_meshes; i++)
		{
			RenderData* mesh = new RenderData;
			std::string file_name = mesh_root_node->mChildren[i]->mName.C_Str();
			file_name.append(".mjope");


			//Indices data
			mesh->num_indices = scene->mMeshes[i]->mNumFaces * 3;
			mesh->indices = new uint[mesh->num_indices];
			for (uint j = 0; j < scene->mMeshes[i]->mNumFaces; ++j)
			{
				if (scene->mMeshes[i]->mFaces[j].mNumIndices != 3)
				{
					LOGC("WARNING, geometry face with != 3 indices! %d", 0);
				}
				else {
					memcpy(&mesh->indices[j * 3], scene->mMeshes[i]->mFaces[j].mIndices, 3 * sizeof(uint));
				}
			}

			//Vertices data 
			mesh->num_vertices = scene->mMeshes[i]->mNumVertices;
			mesh->vertices = new float[mesh->num_vertices *3];
			memcpy(mesh->vertices, scene->mMeshes[i]->mVertices, sizeof(float) * mesh->num_vertices * 3);

			//TextureCoords data
			mesh->num_tex_vertices = scene->mMeshes[i]->mNumVertices;
			mesh->tex_vertices = new float[mesh->num_tex_vertices * 3];
			memcpy(mesh->tex_vertices, scene->mMeshes[i]->mTextureCoords[0], sizeof(float) * mesh->num_tex_vertices * 3);

			//Normals data
			mesh->num_normals = scene->mMeshes[i]->mNumVertices;
			mesh->normals = new float[mesh->num_normals * 3];
			memcpy(mesh->normals, scene->mMeshes[i]->mNormals, sizeof(float) * mesh->num_normals * 3);



			//NumIndices, NumVertices, NumTexCoords, NumNormals
			uint ranges[4] = { mesh->num_indices, mesh->num_vertices,mesh->num_tex_vertices,  mesh->num_normals };


			uint buffer_size = sizeof(ranges) + 
				(sizeof(uint) * mesh->num_indices) +			//Indices
				(sizeof(float) * mesh->num_vertices * 3) +		//Vertices
				(sizeof(float) * mesh->num_tex_vertices * 3) +	//Texture_coords
				(sizeof(float) * mesh->num_normals * 3);		//Normals


			char* buffer_data = new char[buffer_size];
			char* cursor = buffer_data;

			uint bytes_to_copy = sizeof(ranges);
			memcpy(cursor, ranges, bytes_to_copy);
			cursor += bytes_to_copy; //Advance cursor

			bytes_to_copy = (sizeof(uint) * mesh->num_indices);
			memcpy(cursor, mesh->indices, bytes_to_copy);
			cursor += bytes_to_copy; //Advance cursor

			bytes_to_copy = (sizeof(float) * mesh->num_vertices * 3);
			memcpy(cursor, mesh->vertices, bytes_to_copy);
			cursor += bytes_to_copy; //Advance cursor

			bytes_to_copy = (sizeof(float) * mesh->num_tex_vertices * 3);
			memcpy(cursor, mesh->tex_vertices, bytes_to_copy);
			cursor += bytes_to_copy; //Advance cursor

			bytes_to_copy = (sizeof(float) * mesh->num_normals * 3);
			memcpy(cursor, mesh->normals, bytes_to_copy);


			std::ofstream new_file((import_path + file_name).c_str(), std::ofstream::binary);
			new_file.write(buffer_data, buffer_size);

			LOGC("File Saved at: %s", (import_path + file_name).c_str());
		}
	}
}

RenderData * MeshImporter::Load(const char * full_path)
{
	RenderData* ret = new RenderData;
	char* buffer_data = nullptr;

	std::ifstream loaded_file(full_path, std::ifstream::binary);
	if (loaded_file)
	{
		loaded_file.seekg(0, loaded_file.end);
		int buffer_size = loaded_file.tellg();
		loaded_file.seekg(0, loaded_file.beg);

		buffer_data = new char[buffer_size];

		loaded_file.read(buffer_data, buffer_size);

		loaded_file.close();
	}
	
	char* cursor = buffer_data;
	uint bytes_to_copy = 0;

	//Load ranges: indices/vertices/tex_coords/normals
	uint ranges[4];
	bytes_to_copy = sizeof(ranges);
	memcpy(ranges, cursor, bytes_to_copy);
	cursor += bytes_to_copy;

	ret->num_indices = ranges[0];
	ret->num_vertices = ranges[1];
	ret->num_tex_vertices = ranges[2];
	ret->num_normals = ranges[3];

	
	//Load indices
	bytes_to_copy = sizeof(uint) * ret->num_indices;
	ret->indices = new uint[ret->num_indices];
	memcpy(ret->indices, cursor, bytes_to_copy);
	cursor += bytes_to_copy;

	//Load vertices
	bytes_to_copy = sizeof(float) * ret->num_vertices * 3;
	ret->vertices = new float[ret->num_vertices*3];
	memcpy(ret->vertices, cursor, bytes_to_copy);
	cursor += bytes_to_copy;

	//Load tex_coords
	bytes_to_copy = sizeof(float) * ret->num_tex_vertices * 3;
	ret->tex_vertices = new float[ret->num_tex_vertices*3];
	memcpy(ret->tex_vertices, cursor, bytes_to_copy);
	cursor += bytes_to_copy;

	//Load normals
	bytes_to_copy = sizeof(float) * ret->num_normals * 3;
	ret->normals = new float[ret->num_normals*3];
	memcpy(ret->normals, cursor, bytes_to_copy);



	// Load Vertices and Indices To Buffer and Set ID
	if (ret->vertices != nullptr)
	{
		glGenBuffers(1, (GLuint*)&ret->id_vertices);
		glBindBuffer(GL_ARRAY_BUFFER, ret->id_vertices);
		glBufferData(GL_ARRAY_BUFFER, ret->num_vertices * 3 * sizeof(float), &ret->vertices[0], GL_STATIC_DRAW);
	}

	if (ret->normals != nullptr)
	{
		glGenBuffers(1, (GLuint*)&ret->id_normals);
		glBindBuffer(GL_ARRAY_BUFFER, ret->id_normals);
		glBufferData(GL_ARRAY_BUFFER, ret->num_normals * 3 * sizeof(float), &ret->normals[0], GL_STATIC_DRAW);
	}

	if (ret->indices != nullptr)
	{
		glGenBuffers(1, (GLuint*)&ret->id_indices);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ret->id_indices);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, ret->num_indices * sizeof(uint), &ret->indices[0], GL_STATIC_DRAW);
	}

	// Load texture coords buffer
	if (ret->tex_vertices != nullptr)
	{
		glGenBuffers(1, (GLuint*)&ret->id_tex_vertices);
		glBindBuffer(GL_ARRAY_BUFFER, ret->id_tex_vertices);
		glBufferData(GL_ARRAY_BUFFER, ret->num_tex_vertices * 3 * sizeof(float), &ret->tex_vertices[0], GL_STATIC_DRAW);
	}

	//Clean Buffer bind
	glBindBuffer(GL_ARRAY_BUFFER, 0);


	return ret;
}
