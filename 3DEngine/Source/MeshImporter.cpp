#include "MeshImporter.h"
#include "GameObject.h"
#include "ModuleSceneIntro.h"
#include "ModuleResources.h"
#include "Application.h"
#include "Mesh.h"
#include "ResourceMesh.h"
#include "ResourceTexture.h"
#include "ResourceScene.h"
#include "Transform.h"
#include "MeshRenderer.h"
#include "Material.h"
#include "Globals.h"
#include "Math.h"
#include "Glew\include\glew.h"

#include "Importer.h"
#include "TextureImporter.h"

#include "parson.h"
#include "ConfigJSON.h"

#include <filesystem>
#include <fstream>

#include "Assimp/include/cimport.h"
#include "Assimp/include/scene.h"
#include "Assimp/include/postprocess.h"
#include "Assimp/include/cfileio.h"
#include "Assimp/include/version.h"

#pragma comment (lib, "Assimp/libx86/assimp.lib")

MeshImporter::MeshImporter()
{
	//Set the path where all the meshes would be imported
	import_path = JOPE_DATA_DIRECTORY JOPE_LIBRARY_FOLDER JOPE_MESHES_FOLDER;
	assets_meshes_path = JOPE_DATA_DIRECTORY JOPE_ASSETS_FOLDER JOPE_ASSETS_MESH_FOLDER;

}

MeshImporter::~MeshImporter()
{
}

void MeshImporter::Import(const char* full_path, std::string& path, std::string& file_name, std::string& extension)
{
	if (full_path == nullptr) return;

	const aiScene* scene = aiImportFile(full_path , aiProcessPreset_TargetRealtime_MaxQuality);
	aiNode* mesh_root_node = nullptr;
	if (scene == nullptr)
	{
		LOGC("Unable to load the file");
		LOGC("Error: %s", aiGetErrorString());
		return;
	}
	std::map<int, int>* mesh_map = ImportMeshResources(scene, file_name);
	std::map<int, int>* texture_map = ImportTextureResources(scene, path.c_str());

	ImportScene(scene, mesh_map, texture_map, file_name);

	delete mesh_map;
	delete texture_map;
}

void MeshImporter::ImportScene(const aiScene * scene, std::map<int, int>* id_map, std::map<int, int>* text_map, std::string& file_name)
{
	//Dummy game object to save scene
	GameObject* scene_go = new GameObject(nullptr, file_name.c_str());

	aiNode* scene_root_node = scene->mRootNode;

	for (uint i = 0; i < scene_root_node->mNumChildren; i++)
	{
		ImportNode(scene_root_node->mChildren[i], scene, scene_go, scene_root_node->mTransformation, id_map, text_map);
	}
	ResourceScene* scene_resource = (ResourceScene*)App->resources->CreateNewResource(RESOURCE_TYPE::RESOURCE_SCENE);
	scene_resource->SaveResource(scene_go);
	delete scene_go;
}

void MeshImporter::ImportNode(aiNode * to_import, const aiScene * scene, GameObject * import_target, aiMatrix4x4t<float> parent_transform, std::map<int, int>* id_map, std::map<int, int>* text_map)
{
	if (to_import->mNumMeshes > 0)
	{
		for (uint i = 0; i < to_import->mNumMeshes; i++)
		{
			GameObject* node_go = new GameObject(import_target, to_import->mName.C_Str());
			int mesh_id = to_import->mMeshes[i];

			//Assimp id - Resource id map
			std::map<int, int>::const_iterator it = id_map->find(mesh_id);
			if (it == id_map->end())
			{
				LOGC("Mesh resource for %s not found in the database", to_import->mName.C_Str());
				continue;
			}
			int resource_id = it->second;

			//Load node transform
			aiMatrix4x4 curr_trans = parent_transform * to_import->mTransformation;
			float4x4 new_transform(curr_trans.a1, curr_trans.b1, curr_trans.c1, curr_trans.d1,
				curr_trans.a2, curr_trans.b2, curr_trans.c2, curr_trans.d2,
				curr_trans.a3, curr_trans.b3, curr_trans.c3, curr_trans.d3,
				curr_trans.a4, curr_trans.b4, curr_trans.c4, curr_trans.d4);

			node_go->GetTransform()->SetTransform(new_transform);

			Mesh* new_mesh_component = new Mesh(node_go, (ResourceMesh*)App->resources->GetFromUID(resource_id));
			const aiMesh* mesh_material = scene->mMeshes[mesh_id];
			if (mesh_material->HasTextureCoords(0))
			{
				it = text_map->find(mesh_material->mMaterialIndex);
				Material* new_mat_component = new Material(node_go, (ResourceTexture*)App->resources->GetFromUID(it->second));
			}
			
			MeshRenderer* render = new MeshRenderer(node_go);
		}

		//Reset to transform to identidy (they will get the complete transform from the gameobjects parents)
		for (uint i = 0; i < to_import->mNumChildren; i++)
		{
			ImportNode(to_import->mChildren[i], scene, import_target, aiMatrix4x4(), id_map, text_map);
		}
	}
	else
	{
		for (uint i = 0; i < to_import->mNumChildren; i++)
		{
			ImportNode(to_import->mChildren[i], scene, import_target, parent_transform * to_import->mTransformation, id_map, text_map);
		}
	}

}

std::map<int, int>* MeshImporter::ImportMeshResources(const aiScene * scene, std::string& file_name)
{
	std::map<int, int>* ret = new std::map<int,int>;	//Assimp ID - Resource ID
	std::pair<int, int> ret_pair;						//Assimp ID - Resource ID

	Importer* importer = App->resources->GetImporter();
	std::string mesh_path;
	for (uint i = 0; i < scene->mNumMeshes; i++)
	{
		// Path to Assets/ Meshes / File
		mesh_path = assets_meshes_path + file_name + std::to_string(i) + MJOPE;

		ResourceMesh* mesh_resource = nullptr;
		bool meta_found = true;
		if (!importer->FoundMetaFile((mesh_path + METAFORMAT).c_str())) {
			meta_found = false;
			mesh_resource = (ResourceMesh*)App->resources->CreateNewResource(RESOURCE_TYPE::RESOURCE_MESH);
			mesh_resource->SetName((file_name + std::to_string(i)));
		}
		
		Config_Json meta_file((mesh_path + METAFORMAT).c_str());
		if (mesh_resource == nullptr)
			mesh_resource = (ResourceMesh*)App->resources->GetFromUID(meta_file.GetInt("UUID"));
		
		if (!meta_found || importer->NeedReImport(mesh_path.c_str(), meta_file)) {
			RenderData* mesh = new RenderData();

			//Indices data
			mesh->num_indices = scene->mMeshes[i]->mNumFaces * 3;
			mesh->indices = new uint[mesh->num_indices];
			for (uint j = 0; j < scene->mMeshes[i]->mNumFaces; ++j)
			{
				if (scene->mMeshes[i]->mFaces[j].mNumIndices != 3)
				{
					mesh->num_indices -= 3;
					LOGC("WARNING, geometry face with != 3 indices! %d", 0);
				}
				else {
					memcpy(&mesh->indices[j * 3], scene->mMeshes[i]->mFaces[j].mIndices, 3 * sizeof(uint));
				}
			}

			//Vertices data 
			mesh->num_vertices = scene->mMeshes[i]->mNumVertices;
			mesh->vertices = new float[mesh->num_vertices * 3];
			memcpy(mesh->vertices, scene->mMeshes[i]->mVertices, sizeof(float) * mesh->num_vertices * 3);

			//TextureCoords data
			if (scene->mMeshes[i]->HasTextureCoords(0))
			{
				mesh->num_tex_vertices = scene->mMeshes[i]->mNumVertices;
				mesh->tex_vertices = new float[mesh->num_tex_vertices * 3];
				memcpy(mesh->tex_vertices, scene->mMeshes[i]->mTextureCoords[0], sizeof(float) * mesh->num_tex_vertices * 3);
			}

			//Normals data
			mesh->num_normals = scene->mMeshes[i]->mNumVertices;
			mesh->normals = new float[mesh->num_normals * 3];
			memcpy(mesh->normals, scene->mMeshes[i]->mNormals, sizeof(float) * mesh->num_normals * 3);

			mesh_resource->SetRenderData(mesh);

			mesh_resource->SaveResource();
			mesh_resource->SetAssetFile(mesh_path.c_str());
			importer->CopyFileToFolder((import_path + mesh_resource->GetLibraryPath()).c_str(), mesh_path.c_str());
			WriteMeshMeta(meta_file, mesh_resource);
			meta_file.SaveToFile((mesh_path + METAFORMAT).c_str());
			
		}
		//Save file 
		//	mesh_resource->SetLibraryFile(SaveMesh(mesh, file_name.c_str()));

		//fill the id map
		ret_pair.first = i;
		ret_pair.second = mesh_resource->GetUID();
		ret->insert(ret_pair);
	}

	return ret;
}

std::map<int, int>* MeshImporter::ImportTextureResources(const aiScene* scene, const char * full_path)
{
	std::map<int, int>* ret = new std::map<int, int>;	//Assimp ID - Resource ID
	std::pair<int, int> ret_pair;						//Assimp ID - Resource ID

	std::string file_path = full_path;
	for (uint i = 0; i < scene->mNumMaterials; i++)
	{
		int uid = 0;
		aiString file_name;
		
		if (scene->mMaterials[i]->GetTexture(aiTextureType_DIFFUSE, 0, &file_name) == AI_SUCCESS)
		{
			uid = App->resources->GetImporter()->ImportTexture((file_path + file_name.data).c_str(), file_name.data, true);
		}
		else
		{
			LOGC("Unable to get texture");
		}

		ret_pair.first = i;
		ret_pair.second = uid;
		ret->insert(ret_pair);
	}

	return ret;
}

void MeshImporter::WriteMeshMeta(Config_Json & meta_file, const ResourceMesh * resource) const
{
	meta_file.SetInt("UUID", resource->GetUID());

	// Update Creation Time if meta_file created
	int creation_time = meta_file.GetInt("Creation Time");
	if (creation_time == 0) {
		creation_time = (uint)std::chrono::system_clock::to_time_t(std::experimental::filesystem::v1::last_write_time(resource->GetAssetsPath()));
		meta_file.SetInt("Creation Time", creation_time);
	}

	Config_Json texture_importer = meta_file.GetJsonObject("MeshImporter");
	if (texture_importer.config_obj == NULL)
		texture_importer = meta_file.AddJsonObject("MeshImporter");
}


RenderData * MeshImporter::Load(const char * full_path)
{
	RenderData* ret = new RenderData;
	std::string path = full_path;
	//ret->mesh_path = new char[path.size() + 1];
	//memcpy(ret->mesh_path, path.c_str(), path.size());
	//ret->mesh_path[path.size()] = 0x00;
	char* buffer_data = nullptr;

	std::ifstream loaded_file(full_path, std::ifstream::binary);
	if (loaded_file)
	{
		loaded_file.seekg(0, loaded_file.end);
		int buffer_size = (int)loaded_file.tellg();
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

void MeshImporter::GetAssimpVersion(int & major, int & minor, int & revision)
{
	major = aiGetVersionMajor();
	minor = aiGetVersionMinor();
	revision = aiGetVersionRevision();
}

const char* MeshImporter::SaveMesh(RenderData * mesh, const char* file_name)
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

	delete[] buffer_data;

	return (import_path + file_name).c_str();
}