#include "Importer.h"
#include "MeshImporter.h"
#include "TextureImporter.h"
#include "Globals.h"
#include "Mesh.h"
#include "ModuleSceneIntro.h"
#include "Application.h"

#include "Resource.h"
#include "ResourceTexture.h"

#include <filesystem>
#include <fstream>

#include "parson.h"
#include "ConfigJSON.h"

namespace fs = std::experimental::filesystem;

Importer::Importer()
{
	CheckDirectories();
	mesh_importer = new MeshImporter();
	text_importer = new TextureImporter();

}

Importer::~Importer()
{
	delete mesh_importer;
	delete text_importer;
}

void Importer::Import(const char * full_path)
{
	//Chek path isn't a nullptr
	if (full_path == nullptr) return;

	//Divide the full path string into path, name and extension
	std::string path;
	std::string filename;
	std::string extension;

	DividePath((char*)full_path, &path, &filename, &extension);

	//Depending on which file it is decide which importer is needed
	if (extension == ".fbx" || extension == ".obj")
	{
		mesh_importer->Import(full_path, path, filename, extension);
	}
	if (extension == ".png" || extension == ".tga" || extension == ".jpg")
	{
		ImportTexture(full_path, (filename+extension).c_str(), false);
	}
}

void Importer::DividePath(char * full_path, std::string * path, std::string * filename, std::string * extension)
{
	if (full_path == nullptr) return;

	//Make sure the path uses the same slash character
	uint full_path_len = strlen(full_path);
	for (uint i = 0; i < full_path_len; i++)
	{
		full_path[i] = tolower(full_path[i]);

		if (full_path[i] == '\\')
			full_path[i] = '/';
	}

	uint path_end = 0;
	uint name_end = 0;
	
	for (uint i = 0; i < full_path_len; i++)
	{
		if (full_path[i] == '/')
			path_end = i;

		if (full_path[i] == '.')
			name_end = i - 1;
	}

	for (uint i = 0; i < full_path_len; i++)
	{
		if (i <= path_end)
			path->push_back(full_path[i]);
		
		if (i > path_end && i <= name_end)
			filename->push_back(full_path[i]);

		if (i > name_end)
			extension->push_back(full_path[i]);
	}

}

void Importer::NormalizePath(std::string& path)
{
	for (uint i = 0; i < path.size(); i++) {
		if (path[i] == '\\')
			path[i] = '/';
	}
}

void Importer::GetFileName(std::string& file_name)
{
	std::string temp = "";

	uint path_end = 0;
	for (uint i = 0; i < file_name.size(); i++)
	{
		if (file_name[i] == '/') {
			path_end = i;
			temp.clear();
		}
		else temp += file_name[i];
	}

	file_name = temp;
}

int Importer::GetMjopeUID(const char* file_path)
{
	int uid = 0;
	int buffer_size = 0;
	char* buffer_data = nullptr;
	std::ifstream loaded_file(file_path, std::fstream::binary);
	if (loaded_file.good())
	{
		loaded_file.seekg(0, loaded_file.end);
		buffer_size = (uint)loaded_file.tellg();
		loaded_file.seekg(0, loaded_file.beg);

		buffer_data = new char[buffer_size];

		loaded_file.read(buffer_data, buffer_size);
		loaded_file.close();
	}
	else return 0;
	
	char* cursor = buffer_data;
	cursor += 4;
	memcpy(&uid, cursor, sizeof(int));
	delete[] buffer_data;
	return uid;
}

//Create Assts and library directories if don't exist
void Importer::CheckDirectories()
{
	//Data folder
	if (fs::create_directory(JOPE_DATA_DIRECTORY))
	{
		LOGC("Data folder not detected, creating a new one...");
	}
	else
		LOGC("Data folder identified.");

	//Assets folder
	if (fs::create_directory(JOPE_DATA_DIRECTORY JOPE_ASSETS_FOLDER))
	{
		LOGC("Assets not detected, creating a new one...");
	}
	else
		LOGC("Assets folder identified.");

	//Assets/Fbx folder
	assets_fbx_path = JOPE_DATA_DIRECTORY JOPE_ASSETS_FOLDER JOPE_ASSETS_FBX_FOLDER;
	if (fs::create_directory(assets_fbx_path.c_str()))
	{
		LOGC("Assets Fbx not detected, creating a new one...");
	}
	else
		LOGC("Assets Fbx folder identified.");
	
	
	//Assets/Meshes folder
	if (fs::create_directory(JOPE_DATA_DIRECTORY JOPE_ASSETS_FOLDER JOPE_ASSETS_MESH_FOLDER))
	{
		LOGC("Assets Meshes not detected, creating a new one...");
	}
	else
		LOGC("Assets Meshes folder identified.");

	
	//Assets/textures folder
	assets_texture_path = JOPE_DATA_DIRECTORY JOPE_ASSETS_FOLDER JOPE_ASSETS_TEXTURE_FOLDER;
	if (fs::create_directory(assets_texture_path.c_str()))
	{
		LOGC("Assets textures not detected, creating a new one...");
	}
	else
		LOGC("Assets textures folder identified.");


	//Library folder
	if (fs::create_directory(JOPE_DATA_DIRECTORY JOPE_LIBRARY_FOLDER))
	{
		LOGC("Library not detected, creating a new one...");
	}
	else
		LOGC("Library folder identified.");

	//Library/Meshes folder
	if (fs::create_directory(JOPE_DATA_DIRECTORY JOPE_LIBRARY_FOLDER JOPE_MESHES_FOLDER))
	{
		LOGC("Library meshes not detected, creating a new one...");
	}
	else
		LOGC("Library meshes folder identified.");

	//Library/Textures folder
	if (fs::create_directory(JOPE_DATA_DIRECTORY JOPE_LIBRARY_FOLDER JOPE_TEXTURE_FOLDER))
	{
		LOGC("Library textures not detected, creating a new one...");
	}
	else
		LOGC("Library meshes folder identified.");


}

void Importer::CopyFileToFolder(const char * prev_folder, const char * folder) const
{
	std::ifstream in(prev_folder, std::ofstream::binary);
	if (in.good() && in.is_open()) {
		
		in.seekg(0, in.end);
		int lenght = (int)in.tellg();
		in.seekg(0, in.beg);
		
		char* buffer = new char[lenght];

		in.read(buffer, lenght);
		in.close();

		if (buffer == NULL) {
			delete[] buffer;
			return;
		}

		std::ofstream outfile(folder, std::ofstream::binary);

		if (outfile.good())
		{
			// write to outfile
			outfile.write(buffer, lenght);
			outfile.close();
		}

		delete[] buffer;
	}
	
}

bool Importer::FoundMetaFile(const char* meta_file)
{
	if (!std::experimental::filesystem::exists(meta_file)) {
		std::ofstream new_file(meta_file, std::ofstream::binary);
		if (new_file.good()) {
			new_file.write("{}", 2);
		}
		return false;
	}
	return true;
}

bool Importer::NeedReImport(const char * file_path, Config_Json& meta_file)
{
	if (!fs::exists(file_path)) return false;

	uint time = GetLastTimeWritten(file_path);
	uint curr_time = meta_file.GetInt("Creation Time", 0);

	// Check if file have changed
	if (curr_time != time) {
		return true;
	}
	else return false;
}

int Importer::GetLastTimeWritten(const char * assets_file)
{
	if(fs::exists(assets_file))
		return  (int)std::chrono::system_clock::to_time_t(fs::v1::last_write_time(assets_file));
	else return 0;
}

int Importer::ImportTexture(const char * full_path, const char* name, bool from_scene)
{
	std::string path = full_path;
	std::string file_name = name;

	// Normalize Strings to use them
	bool found_file = false;
	if (from_scene) {
		NormalizePath(path);
		NormalizePath(file_name);
		GetFileName(file_name);
		path = path + file_name;
	}

	// Copy texture to Assets/textures folder 
	std::string assets_path = assets_texture_path + file_name.c_str();
	assets_path.append(METAFORMAT);

	// Look for MetaFile/resource or create one if not found
	ResourceTexture* resource = nullptr;
	if (!FoundMetaFile(assets_path.c_str())) {
		resource = (ResourceTexture*)App->resources->CreateNewResource(RESOURCE_TYPE::RESOURCE_TEXTURE);
	} 

	// if meta found get resource by UUID
	Config_Json meta_file(assets_path.c_str());
	if (resource == nullptr)
		resource = (ResourceTexture*)App->resources->GetFromUID(meta_file.GetInt("UUID"));

	// Add Default UUID, and Creation Time if it's a new meta file
	meta_file.SetInt("UUID", resource->GetUID());

	// Import texture and modify meta file
	if (NeedReImport(path.c_str(), meta_file)) {
		CopyFileToFolder(path.c_str(), (assets_texture_path + file_name).c_str());
		text_importer->Import(resource, (assets_texture_path + file_name).c_str(), file_name.c_str(), meta_file);
		resource->ResourceModified();
	}
	meta_file.SetInt("Creation Time", GetLastTimeWritten((assets_texture_path + file_name).c_str()));
	
	meta_file.SaveToFile(assets_path.c_str());
	
	return resource->GetUID();
}

RenderData* Importer::GetNewMesh(const char* mesh_path) const
{
	return mesh_importer->Load(mesh_path);
}
