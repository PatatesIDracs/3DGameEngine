#ifndef __RESOURCEPREFAB_H__
#define __RESOURCEPREFAB_H__

#include "Resource.h"

class GameObject;

//Right now is mostly a copy & paste from module scene
//Done so we can easly add more options to prefabs or scene easily in later versions of the engine
class ResourcePrefab : public Resource
{
public:
	ResourcePrefab(int uid);
	~ResourcePrefab();

	void SaveResource(GameObject* scene_root_go);
	void GetBufferSize(uint& buffer_size);
	void LoadResourceFromBuffer(char* cursor, int& bytes_copied, uint buffer_size);
};

#endif // !__RESOURCEPREFAB_H__

