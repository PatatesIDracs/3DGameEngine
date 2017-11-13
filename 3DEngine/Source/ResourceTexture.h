#ifndef __RESOURCETEXTURE_H__
#define __RESOURCETEXTURE_H__

#include "Resource.h"

enum FORMAT {
	FORM_COLOR_INDEX,
	FORM_RGB,
	FORM_RGBA,
	FORM_BGR,
	FORM_BGRA,
	FORM_LUMINANCE,
	FORM_UNKNOWN
};

class ResourceTexture : public Resource
{
public:
	ResourceTexture(int uid);
	~ResourceTexture() {};

	void LoadToMemory();
	void UnloadFromMemory();

	uint GetTextureID()const;

public:
	uint width = 0;
	uint height = 0;
	uint depth = 0;
	uint mips = 0;
	uint bytes = 0;
	uint texture_id = 0;
	FORMAT format = FORM_UNKNOWN;
};

#endif // !__RESOURCESCENE_H__
