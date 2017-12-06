#ifndef __IMPORTER_H__
#define __IMPORTER_H__

#include <string>

class MeshImporter;
class TextureImporter;
struct RenderData;
class Config_Json;

class Importer
{
public:
	Importer();
	~Importer();

	void Import(const char* full_path);
	TextureImporter*	GetTextureImporter();

	void DividePath(char* full_path, std::string* path, std::string* filename, std::string* extension);
	void NormalizePath(std::string& path);

	void GetFileName(std::string& file_name);
	int  GetMjopeUID(const char* file_path);
	
	void CheckDirectories();

	void CopyFileToFolder(const char* prev_folder, const char* folder) const;

	// meta methods
	bool FoundMetaFile(const char* meta_path);
	bool NeedReImport(const char* meta_path, Config_Json& meta_file);
	int GetLastTimeWritten(const char* assets_file);

	int ImportTexture(const char* full_path, const char* file_name, bool from_scene = true);

	//Mesh importing and loading methods
	RenderData* GetNewMesh(const char* mesh_path) const;

private:

	//Specialized importers
	MeshImporter* mesh_importer = nullptr;
	TextureImporter* text_importer = nullptr;

	std::string assets_fbx_path;
	std::string assets_texture_path;
};

#endif // !__IMPORTER_H__