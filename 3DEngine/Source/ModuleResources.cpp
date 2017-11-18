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
	SearchForResources();

	return true;
}

bool ModuleResources::Start()
{
	// Update Library

	// Load Assets
	LoadFromAssets();

	return true;
}

update_status ModuleResources::Update(float dt)
{

	return UPDATE_CONTINUE;
}

bool ModuleResources::CleanUp()
{
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

	for (; it != end; it++)
	{
		if (fs::is_directory(it->path()))
		{
			LOGC("Folder %S located", it->path().c_str());
		}   
		else
		{
			LOGC("File %S identified", it->path().c_str());
		}

	}
}

void ModuleResources::LoadFromAssets()
{
	fs::recursive_directory_iterator it{ JOPE_DATA_DIRECTORY JOPE_ASSETS_FOLDER };
	fs::recursive_directory_iterator end{};

	std::string path;
	std::string filename;
	std::string extension;
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
			if (extension == METAFORMAT) {
				CheckMetaFiles((path + filename), extension.c_str());
				LOGC("Found Meta File %S File", it->path().c_str());
			}
		}
		path.clear();
		filename.clear();
		extension.clear();

	}
}

bool ModuleResources::CheckMetaFiles(std::string& file_path, const char* extension)
{
	Config_Json meta_file((file_path + extension).c_str());
	Resource* meta_resource = GetFromUID(meta_file.GetInt("UUID"));

	if (!fs::exists(file_path.c_str())) {
		fs::remove((file_path + extension).c_str());
		if (meta_resource != nullptr)
			fs::remove(meta_resource->GetLibraryPath());
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

	//TODO: Check Meta
}

int ModuleResources::Find(const char * file_in_assets) const
{
	//TODO: to method
	for (std::map<int, Resource*>::const_iterator it = resources_map.begin(); it != resources_map.end(); it++)
	{
		
	}

	return 0;
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
