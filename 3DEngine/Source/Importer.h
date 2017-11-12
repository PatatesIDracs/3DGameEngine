#ifndef __IMPORTER_H__
#define __IMPORTER_H__

#include <string>

class MeshImporter;
class TextureImporter;
struct RenderData;

class Importer
{
public:
	Importer();
	~Importer();

	void Import(char* full_path, std::string& new_file);

	void DividePath(char* full_path, std::string* path, std::string* filename, std::string* extension);
	
	void CheckDirectories();
	
	const MeshImporter* GetMeshImporter()const;
	const TextureImporter* GetTextImporter()const;

	//Mesh importing and loading methods
	RenderData* GetNewMesh(const char* mesh_path) const;

private:

	//Specialized importers
	MeshImporter* mesh_importer = nullptr;
	TextureImporter* text_importer = nullptr;
};

#endif // !__IMPORTER_H__