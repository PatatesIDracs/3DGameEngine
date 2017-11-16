#ifndef __RESOURCESCENE_H__
#define __RESOURCESCENE_H__

#include "Resource.h"

class GameObject;

class ResourceScene : public Resource
{
public:
	ResourceScene(int uid);
	~ResourceScene();

	void SaveResource(GameObject* scene_root_go);
	void GetBufferSize(uint& buffer_size);
	void LoadResource(char* cursor, int& bytes_copied);

};

#endif // !__RESOURCESCENE_H__

