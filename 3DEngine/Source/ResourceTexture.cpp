#include "ResourceTexture.h"

#include "Devil\include\il.h"
#include "Devil\include\ilu.h"
#include "Devil\include\ilut.h"

ResourceTexture::ResourceTexture(int uid): Resource(uid)
{
	type = RESOURCE_TYPE::RESOURCE_TEXTURE;
}

void ResourceTexture::LoadToMemory()
{
	if (texture_id == 0 && library_file.size() > 0) {
		texture_id = ilutGLLoadImage((char*)library_file.c_str());
	}
}

uint ResourceTexture::GetTextureID() const
{
	return texture_id;
}
