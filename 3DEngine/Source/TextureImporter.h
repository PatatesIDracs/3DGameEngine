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

	int LoadToBuffer(const char* path);

	void Import(ResourceTexture* resource, const char* full_path, const char* name, Config_Json& meta_file);
	
	void WriteTextureMeta(Config_Json& meta_file, const ResourceTexture* resource) const;

	int GetDevilVersion();

private:
	std::string import_path;
	std::string assets_path;
};

#endif // !__TEXTUREIMPORTER_H__