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


TextureImporter::TextureImporter()
{
	import_path = JOPE_DATA_DIRECTORY JOPE_LIBRARY_FOLDER JOPE_TEXTURE_FOLDER;
	ilInit();
	iluInit();
	ilutInit();
	ilutRenderer(ILUT_OPENGL);
}

TextureImporter::~TextureImporter()
{
}

void TextureImporter::Import(ResourceTexture* resource, const char * full_path, const char* name)
{
	resource->SetName(name);
	std::string file_name = name;
	file_name.append(TEXFILEFORMAT);

	//Generate file
	ilEnable(IL_FILE_OVERWRITE);
	uint imgID = 0;
	ilGenImages(1, &imgID);
	ilBindImage(imgID);
	ILboolean success = ilLoadImage(full_path);

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

			resource->SetLibraryFile((import_path + file_name).c_str());
		}
	}
	resource->SetAssetFile((full_path + file_name).c_str());
}