#include "ResourceScene.h"

ResourceScene::ResourceScene(int uid, RESOURCE_TYPE type) : Resource(uid)
{
	type = RESOURCE_TYPE::RESOURCE_SCENE;
}

ResourceScene::~ResourceScene()
{
}
