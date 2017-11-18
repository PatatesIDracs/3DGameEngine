#include "TextureImporter.h"
#include "Globals.h"
#include <fstream>

#include "Resource.h"
#include "ResourceTexture.h"

#include "Devil\include\il.h"
#include "Devil\include\ilu.h"
#include "Devil\include\ilut.h"

#pragma comment( lib, "Devil/libx86/DevIL.lib" )
#pragma comment( lib, "Devil/libx86/ILU.lib" )
#pragma comment( lib, "Devil/libx86/ILUT.lib" )

#include "parson.h"
#include "ConfigJSON.h"

TextureImporter::TextureImporter()
{
	import_path = JOPE_DATA_DIRECTORY JOPE_LIBRARY_FOLDER JOPE_TEXTURE_FOLDER;
	assets_path = JOPE_DATA_DIRECTORY JOPE_ASSETS_FOLDER JOPE_ASSETS_TEXTURE_FOLDER;
	ilInit();
	iluInit();
	ilutInit();
	ilutRenderer(ILUT_OPENGL);
}

TextureImporter::~TextureImporter()
{
	import_path.clear();
	assets_path.clear();
}

void TextureImporter::Import(ResourceTexture* resource, const char * path, const char* name, Config_Json& meta_file)
{
	// Get Full Path
	std::string file_name = name;
	resource->SetName(name);

	// Add .dds File Format
	file_name.append(TEXFILEFORMAT);

	//Generate file
	ilEnable(IL_FILE_OVERWRITE);
	uint imgID = 0;
	ilGenImages(1, &imgID);
	ilBindImage(imgID);
	ILboolean success = ilLoadImage(path);

	ILuint size;
	ILubyte* data;

	ilSetInteger(IL_DXTC_FORMAT, IL_DXT5);
	size = ilSaveL(IL_DDS, NULL, 0);
	if (size > 0)
	{
		data = new ILubyte[size];
		if (ilSaveL(IL_DDS, data, size) > 0)
		{
			std::ofstream new_file((import_path + file_name).c_str(), std::ofstream::binary);
			new_file.write((char*)data, size);

			LOGC("Imported texture as %s", file_name.c_str(), import_path.c_str());			

			// Set Resource data
			ILinfo info;
			iluGetImageInfo(&info);
			resource->width = info.Width;
			resource->height = info.Height;
			resource->depth = info.Depth;
			resource->mips = info.NumMips;
			resource->bytes = info.Bpp;
			resource->format = (FORMAT)((int)ilGetInteger(info.Format));
		}
		delete[] data;
	}
	resource->SetLibraryFile((import_path + file_name).c_str());
	resource->SetAssetFile((assets_path + name).c_str());

	// Write Texture Meta File
	WriteTextureMeta(meta_file, resource);
}

void TextureImporter::WriteTextureMeta(Config_Json& meta_file, const ResourceTexture* resource) const
{
	meta_file.SetInt("UUID", resource->GetUID());
	
	Config_Json texture_importer = meta_file.GetJsonObject("TextureImporter");
	if (texture_importer.config_obj == NULL)
		texture_importer = meta_file.AddJsonObject("TextureImporter");

	//texture_importer.SetInt("DX compression", ilGetInteger(IL_DXTC_DATA_FORMAT));

}

int TextureImporter::GetDevilVersion()
{
	return IL_VERSION;
}
