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
	void LoadResourceFromBuffer(char* cursor, int& bytes_copied, uint buffer_size);

	void SetAsRoot(bool new_state);

private:
	//Define if it is a root_scene or not
	//Not root_scenes are treated as prefabs (game objects that can be loaded with all its childs)
	//Root_scenes will replace the current scene in the editor
	bool root_scene = false;

};

#endif // !__RESOURCESCENE_H__

