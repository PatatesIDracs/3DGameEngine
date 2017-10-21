#ifndef __TEXTUREIMPORTER_H__
#define __TEXTUREIMPORTER_H__

class TextureImporter
{
public:
	TextureImporter();
	~TextureImporter();

	void Import(const char* full_path);

};

#endif // !__TEXTUREIMPORTER_H__