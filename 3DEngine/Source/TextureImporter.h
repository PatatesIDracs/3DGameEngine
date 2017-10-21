#ifndef __TEXTUREIMPORTER_H__
#define __TEXTUREIMPORTER_H__

#include <string>

#define TEXFILEFORMAT ".dds"

class TextureImporter
{
public:
	TextureImporter();
	~TextureImporter();

	void Import(const char* full_path, const char* name);

private:
	std::string import_path;
};

#endif // !__TEXTUREIMPORTER_H__