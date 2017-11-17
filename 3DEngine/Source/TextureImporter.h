#ifndef __TEXTUREIMPORTER_H__
#define __TEXTUREIMPORTER_H__

#include <string>

#define TEXFILEFORMAT ".dds"

class ResourceTexture;
class Config_Json;

class TextureImporter
{
public:
	TextureImporter();
	~TextureImporter();

	//void GetFilePath(std::string& file_name);
	//void GetFileName(std::string& file_name);

	void Import(ResourceTexture* resource, const char* full_path, const char* name, Config_Json& meta_file);
	//void ReImport(ResourceTexture* resource, Config_Json& meta_file);

	void WriteMeta(Config_Json& meta_file, const ResourceTexture* resource) const;

private:
	std::string import_path;
	std::string assets_path;
};

#endif // !__TEXTUREIMPORTER_H__