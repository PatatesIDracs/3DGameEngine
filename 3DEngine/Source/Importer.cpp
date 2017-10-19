#include "Importer.h"
#include "MeshImporter.h"
#include "Globals.h"


Importer::Importer()
{
	mesh_importer = new MeshImporter();
}

Importer::~Importer()
{
}

void Importer::Import(char * full_path)
{
	//Chek path isn't a nullptr
	if (full_path == nullptr) return;

	//Divide the full path string into path, name and extension
	std::string path;
	std::string filename;
	std::string extension;

	DividePath(full_path, &path, &filename, &extension);

	//Depending on which file it is decide which importer is needed
	if (extension == ".fbx")
		mesh_importer->Import(&path, &filename, &extension);
}

void Importer::DividePath(char * full_path, std::string * path, std::string * filename, std::string * extension)
{
	if (full_path == nullptr) return;

	//Make sure the path uses the same slash character
	uint full_path_len = strlen(full_path);
	for (int i = 0; i < full_path_len; i++)
	{
		if (full_path[i] == '/')
			full_path[i] = '\\';
	}

	uint path_end = 0;
	uint name_end = 0;
	
	for (int i = 0; i < full_path_len; i++)
	{
		if (full_path[i] == '\\')
			path_end = i;

		if (full_path[i] == '.')
			name_end = i - 1;
	}

	for (int i = 0; i < full_path_len; i++)
	{
		if (i <= path_end)
			path->push_back(full_path[i]);
		
		if (i > path_end && i <= name_end)
			filename->push_back(full_path[i]);

		if (i > name_end)
			extension->push_back(full_path[i]);
	}

}
