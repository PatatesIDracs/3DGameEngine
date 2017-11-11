#include "Importer.h"
#include "MeshImporter.h"
#include "TextureImporter.h"
#include "Globals.h"
#include "Mesh.h"
#include "ModuleSceneIntro.h"
#include "Application.h"

#include <filesystem>

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

void Importer::Import(char * full_path, std::string& new_file)
{
	//Chek path isn't a nullptr
	if (full_path == nullptr) return;

	//Divide the full path string into path, name and extension
	std::string path;
	std::string filename;
	std::string extension;

	DividePath(full_path, &path, &filename, &extension);

	//Depending on which file it is decide which importer is needed
	if (extension == ".fbx" || extension == ".obj")
	{
		//Create a new gameobject to store the data
		GameObject* imported_go = App->scene_intro->CreateNewGameObject(filename.c_str());

		mesh_importer->Import(full_path, imported_go);
	}
	if (extension == ".png")
	{
		text_importer->Import(full_path, filename.c_str(), new_file);
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

//Create Assts and library directories if don't exist
void Importer::CheckDirectories()
{
	//Assets folder
	if (std::experimental::filesystem::create_directory(JOPE_DATA_DIRECTORY JOPE_ASSETS_FOLDER))
	{
		LOGC("Assets not detected, creating a new one...");
	}
	else
		LOGC("Assets folder identified.");


	//Library folder
	if (std::experimental::filesystem::create_directory(JOPE_DATA_DIRECTORY JOPE_LIBRARY_FOLDER))
	{
		LOGC("Library not detected, creating a new one...");
	}
	else
		LOGC("Library folder identified.");

	//Library/Meshes folder
	if (std::experimental::filesystem::create_directory(JOPE_DATA_DIRECTORY JOPE_LIBRARY_FOLDER JOPE_MESHES_FOLDER))
	{
		LOGC("Library meshes not detected, creating a new one...");
	}
	else
		LOGC("Library meshes folder identified.");

	//Library/Textures folder
	if (std::experimental::filesystem::create_directory(JOPE_DATA_DIRECTORY JOPE_LIBRARY_FOLDER JOPE_TEXTURE_FOLDER))
	{
		LOGC("Library textures not detected, creating a new one...");
	}
	else
		LOGC("Library meshes folder identified.");


}

RenderData* Importer::GetNewMesh(const char* mesh_path) const
{
	return mesh_importer->Load(mesh_path);
}
