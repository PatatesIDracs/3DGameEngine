#ifndef __RESOURCES_H__
#define __RESOURCES_H__

#include "Globals.h"
#include <string>

enum RESOURCE_TYPE
{
	RESOURCE_UNKNOW = 0,
	RESOURCE_MESH,
	RESOURCE_TEXTURE,
	RESOURCE_SCENE
};

class Resource
{
public:
	Resource(int uid);
	virtual ~Resource();

	int GetUID();

	void SetFolders(std::string assets_file, std::string library_file);

protected:
	int uid = 0;
	RESOURCE_TYPE type = RESOURCE_UNKNOW;
	uint loaded = 0;
	std::string assets_file;
	std::string library_file;

};


#endif // !__RESOURCES_H__