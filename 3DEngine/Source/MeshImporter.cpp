#include "MeshImporter.h"
#include "Mesh.h"

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

void MeshImporter::Import(std::string* path, std::string* filename, std::string* extension)
{
	if (path == nullptr) return;

	const aiScene* scene = aiImportFile((*path + *filename + *extension).c_str() , aiProcessPreset_TargetRealtime_MaxQuality);
	aiNode* parent = scene->mRootNode;
	if (scene != nullptr && scene->HasMeshes())
	{
		uint num_meshes = scene->mNumMeshes;
		for (uint i = 0; i < num_meshes; i++)
		{
			RenderData* mesh = new RenderData;
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
			memcpy(mesh->vertices, scene->mMeshes[i]->mNormals, sizeof(float) * mesh->num_normals * 3);



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
			memcpy(buffer_data, ranges, bytes_to_copy);
			cursor += bytes_to_copy; //Advance cursor

			bytes_to_copy = (sizeof(uint) * mesh->num_indices);
			memcpy(buffer_data, mesh->indices, bytes_to_copy);
			cursor += bytes_to_copy; //Advance cursor

			bytes_to_copy = (sizeof(float) * mesh->num_vertices * 3);
			memcpy(buffer_data, mesh->vertices, bytes_to_copy);
			cursor += bytes_to_copy; //Advance cursor

			bytes_to_copy = (sizeof(float) * mesh->num_tex_vertices * 3);
			memcpy(buffer_data, mesh->tex_vertices, bytes_to_copy);
			cursor += bytes_to_copy; //Advance cursor

			bytes_to_copy = (sizeof(float) * mesh->num_normals * 3);
			memcpy(buffer_data, mesh->normals, bytes_to_copy);
		}

	}
}
