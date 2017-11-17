#include "Importer.h"
#include "MeshImporter.h"
#include "TextureImporter.h"
#include "Globals.h"
#include "Mesh.h"
#include "ModuleSceneIntro.h"
#include "Application.h"

#include "Resource.h"

#include <filesystem>
#include <fstream>

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
		CopyFileToFolder(full_path, (JOPE_DATA_DIRECTORY JOPE_ASSETS_FOLDER JOPE_ASSETS_FBX_FOLDER + filename + extension).c_str());
		mesh_importer->Import(full_path, path, filename, extension);
	}
	if (extension == ".png" || extension == ".tga")
	{
		CopyFileToFolder(full_path, (JOPE_DATA_DIRECTORY JOPE_ASSETS_FOLDER JOPE_ASSETS_TEXTURE_FOLDER + filename + extension).c_str());
		ResourceTexture* new_resource = (ResourceTexture*)App->resources->CreateNewResource(RESOURCE_TYPE::RESOURCE_TEXTURE);
		text_importer->Import(new_resource, path.c_str(), (filename + extension).c_str());
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
	//Data folder
	if (std::experimental::filesystem::create_directory(JOPE_DATA_DIRECTORY))
	{
		LOGC("Data folder not detected, creating a new one...");
	}
	else
		LOGC("Data folder identified.");

	//Assets folder
	if (std::experimental::filesystem::create_directory(JOPE_DATA_DIRECTORY JOPE_ASSETS_FOLDER))
	{
		LOGC("Assets not detected, creating a new one...");
	}
	else
		LOGC("Assets folder identified.");

	//Assets/Fbx folder
	if (std::experimental::filesystem::create_directory(JOPE_DATA_DIRECTORY JOPE_ASSETS_FOLDER JOPE_ASSETS_FBX_FOLDER))
	{
		LOGC("Assets Fbx not detected, creating a new one...");
	}
	else
		LOGC("Assets Fbx folder identified.");

	//Assets/textures folder
	if (std::experimental::filesystem::create_directory(JOPE_DATA_DIRECTORY JOPE_ASSETS_FOLDER JOPE_ASSETS_TEXTURE_FOLDER))
	{
		LOGC("Assets textures not detected, creating a new one...");
	}
	else
		LOGC("Assets textures folder identified.");


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

void Importer::CopyFileToFolder(const char * prev_folder, const char * folder) const
{
	std::ifstream in(prev_folder, std::ofstream::binary);
	if (in.good() && in.is_open()) {
		
		in.seekg(0, in.end);
		int lenght = in.tellg();
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

const MeshImporter * Importer::GetMeshImporter() const
{
	return mesh_importer;
}

const TextureImporter * Importer::GetTextImporter() const
{
	return text_importer;
}

RenderData* Importer::GetNewMesh(const char* mesh_path) const
{
	return mesh_importer->Load(mesh_path);
}
