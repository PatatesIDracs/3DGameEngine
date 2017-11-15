#ifndef __RESOURCESCENE_H__
#define __RESOURCESCENE_H__

#include "Resource.h"

class GameObject;

class ResourceScene : public Resource
{
public:
	ResourceScene(int uid, RESOURCE_TYPE type);
	~ResourceScene();

	void SaveResource(GameObject* scene_root_go);
	void GetBufferSize(uint& buffer_size);


};

#endif // !__RESOURCESCENE_H__

