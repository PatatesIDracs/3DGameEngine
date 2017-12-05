#include "Application.h"
#include "ModuleResources.h"
#include "Importer.h"

#include "Resource.h"
#include "ResourceMesh.h"
#include "ResourceScene.h"
#include "ResourceTexture.h"

#include "parson.h"
#include "ConfigJSON.h"

#include <string>

namespace fs = std::experimental::filesystem;

ModuleResources::ModuleResources(Application* app, bool start_enabled) : Module(app, "Resources", start_enabled)
{
}

ModuleResources::~ModuleResources()
{
}

bool ModuleResources::Init()
{
	jope_importer = new Importer();

	return true;
}

bool ModuleResources::Start()
{
	// Update Library
	SearchForResources();

	// Load Assets
	LoadFromAssets();

	return true;
}

update_status ModuleResources::Update(float dt)
{
	if (update_assets) {
		UpdateAssetsFiles();
		update_assets = false;
	}
	return UPDATE_CONTINUE;
}

bool ModuleResources::CleanUp()
{
	all_resources_vec.clear();
	resources_vec.clear();

	//Delete all resources
	for (std::map<int, Resource*>::const_iterator it = resources_map.begin(); it != resources_map.end(); it++)
	{
		delete it->second;
	}

	delete jope_importer;
	return true;
}

void ModuleResources::SearchForResources()
{
	fs::recursive_directory_iterator it{JOPE_DATA_DIRECTORY JOPE_LIBRARY_FOLDER};
	fs::recursive_directory_iterator end{};

	std::string path;
	std::string filename;
	std::string extension;
	std::string temp;

	//Stored and loaded at the end
	std::vector<ResourceScene*> scenes_to_load;

	for (; it != end; it++)
	{
		if (fs::is_directory(it->path()))
		{
			LOGC("Folder %S located", it->path().c_str());
		}   
		else
		{
			temp = it->path().string();
			jope_importer->DividePath((char*)temp.c_str(), &path, &filename, &extension);

			//Load scene resource
			if (extension == SCENEFORMAT)
			{
				ResourceScene* new_scene = (ResourceScene*)CreateNewResource(RESOURCE_TYPE::RESOURCE_SCENE, std::stoi(filename));
				new_scene->SetLibraryFile(filename + extension);
				scenes_to_load.push_back(new_scene);
				LOGC("Scene %s loaded", it->path().c_str());
			}

			//Load mesh resource
			if (extension == MJOPE)
			{
				ResourceMesh* new_mesh = (ResourceMesh*)CreateNewResource(RESOURCE_TYPE::RESOURCE_MESH, std::stoi(filename));
				new_mesh->SetLibraryFile(JOPE_MESHES_FOLDER + filename + extension);
				new_mesh->LoadResource();
				LOGC("Mesh %s loaded", it->path().c_str());
			}

			//Load textures resource
			if (extension == TEXFORMAT)
			{
				//Save and load
				ResourceTexture* new_texture = (ResourceTexture*)CreateNewResource(RESOURCE_TYPE::RESOURCE_TEXTURE, std::stoi(filename));
				new_texture->SetLibraryFile(JOPE_TEXTURE_FOLDER + filename + extension);
				new_texture->LoadResource();
				LOGC("Texture %s loaded", it->path().c_str());
			}


			LOGC("File %S identified", it->path().c_str());
			path.clear();
			filename.clear();
			extension.clear();
		}

	}
	for (uint i = 0; i < scenes_to_load.size(); i++)
	{
		scenes_to_load[i]->LoadResource();
	}
}

void ModuleResources::LoadFromAssets()
{
	fs::recursive_directory_iterator it{ JOPE_DATA_DIRECTORY JOPE_ASSETS_FOLDER };
	fs::recursive_directory_iterator end{};

	std::string path;
	std::string filename;
	std::string extension;
	std::string meta_path;
	std::string temp;
	for (; it != end; it++)
	{
		if (fs::is_directory(it->path())) {
			LOGC("Folder %S located", it->path().c_str());
		}
		else
		{
			temp = it->path().string();
			jope_importer->DividePath((char*)temp.c_str(), &path, &filename, &extension);

			meta_path = (path + filename + extension + METAFORMAT).c_str();

			if (extension == METAFORMAT) {
				CheckMetaFiles((path + filename), extension.c_str());
				LOGC("Found Meta File %S File", it->path().c_str());
			}
			else if (!fs::exists(meta_path.c_str())) {
				if(extension != MJOPE)
					jope_importer->Import((path + filename + extension).c_str());
				else {
					int UUID = jope_importer->GetMjopeUID((path + filename + extension).c_str());
					temp = std::to_string(UUID);

					// Create Meta file
					jope_importer->FoundMetaFile(meta_path.c_str());
					Config_Json meta_file(meta_path.c_str());
					
					ResourceMesh* new_mesh = nullptr;
					if (!fs::exists((JOPE_DATA_DIRECTORY JOPE_LIBRARY_FOLDER JOPE_MESHES_FOLDER + temp + extension))) {
						// copy file to library
						jope_importer->CopyFileToFolder((path + filename + extension).c_str(), (JOPE_DATA_DIRECTORY JOPE_LIBRARY_FOLDER JOPE_MESHES_FOLDER + temp + extension).c_str());

						// Load Resource
						ResourceMesh* new_mesh = (ResourceMesh*)CreateNewResource(RESOURCE_TYPE::RESOURCE_MESH);
						new_mesh->SetLibraryFile(JOPE_MESHES_FOLDER + temp + extension);
						new_mesh->SetAssetFile(JOPE_ASSETS_MESH_FOLDER + filename + extension);
						new_mesh->LoadResource();
		
						LOGC("Mesh %s loaded", it->path().c_str());
					}
					else {
						new_mesh = (ResourceMesh*)GetFromUID(UUID);
					}

					// Write to Meta
					meta_file.SetInt("UUID", new_mesh->GetUID());
					meta_file.SetInt("Creation Time",jope_importer->GetLastTimeWritten((path + filename + extension).c_str()));
					meta_file.AddJsonObject("MeshImporter");
					meta_file.SaveToFile(meta_path.c_str());
				}
			}
		}
		path.clear();
		filename.clear();
		extension.clear();

	}
}

void ModuleResources::UpdateAssetsFiles()
{
	fs::recursive_directory_iterator it{ JOPE_DATA_DIRECTORY JOPE_ASSETS_FOLDER };
	fs::recursive_directory_iterator end{};

	resources_vec.clear();
	all_resources_vec.clear();
	scene_vec.clear();
	mesh_vec.clear();
	texture_vec.clear();

	std::string path;
	std::string filename;
	std::string extension;
	std::string temp;
	for (; it != end; it++)
	{
		temp = it->path().string();
		if (!fs::is_directory(temp.c_str())) {
			
			jope_importer->DividePath((char*)temp.c_str(), &path, &filename, &extension);

			if (extension == METAFORMAT) {
				Config_Json meta_file(temp.c_str());
				int creation_time = jope_importer->GetLastTimeWritten((path + filename).c_str());
				int time = meta_file.GetInt("Creation Time");
				if (fs::exists(path + filename) && time != creation_time) {
					temp = path + filename;
					path.clear();
					filename.clear();
					extension.clear();
					jope_importer->DividePath((char*)temp.c_str(), &path, &filename, &extension);

					if (extension == ".fbx" || extension == ".obj" || extension == ".png" || extension == ".tga" ||extension == ".jpg") {
						jope_importer->Import((path + filename + extension).c_str());
					}
					LOGC("Updated %s File ", temp.c_str());
				}	

				// Add resource to Assets window
				Resource* load_this = GetFromUID(meta_file.GetInt("UUID"));
				if (load_this != nullptr) {
					all_resources_vec.push_back(load_this);
				}
			}
			else if (fs::exists((temp + METAFORMAT).c_str())) {
				// Add resource to Assets window
				Config_Json meta_file((temp + METAFORMAT).c_str());
				Resource* load_this = GetFromUID(meta_file.GetInt("UUID"));
				
				if (load_this == nullptr) continue;
				if (extension == MJOPE) {
					mesh_vec.push_back(load_this);
				}
				else if (extension == TEXFORMAT) {
					texture_vec.push_back((ResourceTexture*)load_this);
				}
				else if (extension == SCENEFORMAT || extension == ".fbx") {
					scene_vec.push_back(load_this);
				}
			}
			path.clear();
			filename.clear();
			extension.clear();
		}
	}
}

bool ModuleResources::CheckMetaFiles(std::string& file_path, const char* extension)
{
	Config_Json meta_file((file_path + extension).c_str());
	Resource* meta_resource = GetFromUID(meta_file.GetInt("UUID"));

	if (!fs::exists(file_path.c_str())) {
		fs::remove((file_path + extension).c_str());
		if (meta_resource != nullptr) {
			file_path = JOPE_DATA_DIRECTORY JOPE_LIBRARY_FOLDER;
			fs::remove((file_path + meta_resource->GetLibraryPath()).c_str());
		}
	}
	else {
		if (meta_resource == nullptr) {
			jope_importer->Import(file_path.c_str());
		}
	}
	return true;
}

void ModuleResources::HandleDropEvent(SDL_DropEvent drop_event)
{
	jope_importer->Import(drop_event.file);
}

void ModuleResources::GotFocus(bool focus_app)
{
	update_assets = focus_app;
}

void ModuleResources::DeleteFileFromUID(int uid)
{
	std::map<int, Resource*>::const_iterator it = resources_map.find(uid);
	if (it == resources_map.end())
		return;

	std::string to_erase = JOPE_DATA_DIRECTORY JOPE_ASSETS_FOLDER;
	to_erase.append(it->second->GetAssetsPath());
	if (fs::exists(to_erase.c_str()) && fs::is_regular_file(to_erase.c_str()))
		fs::remove(to_erase.c_str());
	to_erase = JOPE_DATA_DIRECTORY JOPE_LIBRARY_FOLDER;
	to_erase.append(it->second->GetLibraryPath());
	if (fs::exists(to_erase.c_str()) && fs::is_regular_file(to_erase.c_str()))
		fs::remove(to_erase.c_str());

}

int ModuleResources::GenerateNewUID()
{
	return App->GetIntUUID();
}

Resource * ModuleResources::GetFromUID(int uid) const
{
	std::map<int, Resource*>::const_iterator it = resources_map.find(uid);

	if (it == resources_map.end())
		return nullptr;
	else
		return it._Ptr->_Myval.second;
}

Resource * ModuleResources::CreateNewResource(RESOURCE_TYPE type, int force_uid)
{
	Resource* ret = nullptr;
	int new_UID = 0;
	if (force_uid == 0)
		new_UID = GenerateNewUID();
	else
		new_UID = force_uid;

	switch (type)
	{
	case RESOURCE_UNKNOW:
		break;
	case RESOURCE_MESH:
		ret = new ResourceMesh(new_UID);
		break;
	case RESOURCE_TEXTURE:
		ret = new ResourceTexture(new_UID);
		break;
	case RESOURCE_SCENE:
		ret = new ResourceScene(new_UID);
		break;
	default:
		break;
	}

	std::pair<int, Resource*> resource_pair;
	resource_pair.first = ret->GetUID();
	resource_pair.second = ret;
	
	resources_map.insert(resource_pair);

	return ret;
}

Importer * ModuleResources::GetImporter() const
{
	return jope_importer;
}
