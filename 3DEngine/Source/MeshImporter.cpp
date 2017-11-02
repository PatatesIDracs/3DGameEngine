#include "MeshImporter.h"
#include "GameObject.h"
#include "ModuleSceneIntro.h"
#include "Application.h"
#include "Mesh.h"
#include "Transform.h"
#include "MeshRenderer.h"
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
	//Set the path where all the meshes would be imported
	import_path = JOPE_DATA_DIRECTORY JOPE_LIBRARY_FOLDER JOPE_MESHES_FOLDER;
}

MeshImporter::~MeshImporter()
{
}

void MeshImporter::Import(const char* full_path, GameObject* import_target)
{
	if (full_path == nullptr) return;

	const aiScene* scene = aiImportFile(full_path , aiProcessPreset_TargetRealtime_MaxQuality);
	aiNode* mesh_root_node = nullptr;
	if (scene != nullptr)
		mesh_root_node = scene->mRootNode;


	if (mesh_root_node != nullptr)
	{
		aiMatrix4x4 node_transform = mesh_root_node->mTransformation;
		ImportNode(mesh_root_node, scene, import_target, node_transform);
	}
	else
		LOGC("No meshes found");
}

void MeshImporter::ImportNode(aiNode * to_import, const aiScene* scene, GameObject* import_target, aiMatrix4x4t<float> parent_transform)
{

	for (int k = 0; k < to_import->mNumChildren; k++)
	{
		aiNode* child_node = to_import->mChildren[k];

		if (child_node->mNumChildren > 0)
			ImportNode(child_node, scene, import_target, (parent_transform * child_node->mTransformation));

		if (child_node->mNumMeshes > 0)
		{

			uint num_meshes = child_node->mNumMeshes;
			if (num_meshes == 1)
			{
				//TODO: Save file one time (without creating child GameObjects), should make a function that puts mesh data in RenderData* and call it
			}

			for (uint i = 0; i < num_meshes; i++)
			{
				int mesh_id = child_node->mMeshes[i];
				RenderData* mesh = new RenderData;
				std::string file_name = child_node->mName.C_Str();
				file_name.append(MESHFILEFORMAT);

				//Indices data
				mesh->num_indices = scene->mMeshes[mesh_id]->mNumFaces * 3;
				mesh->indices = new uint[mesh->num_indices];
				for (uint j = 0; j < scene->mMeshes[mesh_id]->mNumFaces; ++j)
				{
					if (scene->mMeshes[mesh_id]->mFaces[j].mNumIndices != 3)
					{
						LOGC("WARNING, geometry face with != 3 indices! %d", 0);
					}
					else {
						memcpy(&mesh->indices[j * 3], scene->mMeshes[mesh_id]->mFaces[j].mIndices, 3 * sizeof(uint));
					}
				}

				//Vertices data 
				mesh->num_vertices = scene->mMeshes[mesh_id]->mNumVertices;
				mesh->vertices = new float[mesh->num_vertices * 3];
				memcpy(mesh->vertices, scene->mMeshes[mesh_id]->mVertices, sizeof(float) * mesh->num_vertices * 3);

				//TextureCoords data
				if (scene->mMeshes[mesh_id]->HasTextureCoords(0))
				{
					mesh->num_tex_vertices = scene->mMeshes[mesh_id]->mNumVertices;
					mesh->tex_vertices = new float[mesh->num_tex_vertices * 3];
					memcpy(mesh->tex_vertices, scene->mMeshes[mesh_id]->mTextureCoords[0], sizeof(float) * mesh->num_tex_vertices * 3);
				}

				//Normals data
				mesh->num_normals = scene->mMeshes[mesh_id]->mNumVertices;
				mesh->normals = new float[mesh->num_normals * 3];
				memcpy(mesh->normals, scene->mMeshes[mesh_id]->mNormals, sizeof(float) * mesh->num_normals * 3);

				//Save file 
				SaveMesh(mesh, file_name.c_str());

				aiMatrix4x4 curr_trans = parent_transform * child_node->mTransformation;
				float4x4 new_transform(curr_trans.a1, curr_trans.a2, curr_trans.a3, curr_trans.a4,
										curr_trans.b1, curr_trans.b2, curr_trans.b3, curr_trans.b4, 
										curr_trans.c1, curr_trans.c2, curr_trans.c3, curr_trans.c4, 
										curr_trans.d1, curr_trans.d2, curr_trans.d3, curr_trans.d4);

				GameObject* mesh_holder = App->scene_intro->CreateNewGameObject(to_import->mChildren[mesh_id]->mName.C_Str(), import_target);

				mesh_holder->GetTransform()->SetTransform(new_transform);
				//RenderData* new_mesh_data = ;
				Mesh* new_mesh_component = new Mesh(mesh_holder, Load((import_path + file_name).c_str()));
				mesh_holder->AddComponent(new_mesh_component);
				mesh_holder->AddComponent(new MeshRenderer(mesh_holder));

			}
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
	
	if (buffer_data == nullptr)
		return nullptr;

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

	delete[] buffer_data;

	LOGC("Mesh loaded");
	return ret;
}

void MeshImporter::SaveMesh(RenderData * mesh, const char* file_name)
{
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

