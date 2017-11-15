#include "ResourceScene.h"
#include "GameObject.h"

ResourceScene::ResourceScene(int uid, RESOURCE_TYPE type) : Resource(uid)
{
	type = RESOURCE_TYPE::RESOURCE_SCENE;
}

ResourceScene::~ResourceScene()
{
}

void ResourceScene::SaveResource(GameObject* scene_root_go)
{
	uint buffer_size = 0;
	GetBufferSize(buffer_size);
	scene_root_go->GetOwnBufferSize(buffer_size);

	char* buffer_data = new char[buffer_size];
	char* cursor = buffer_data;

	int bytes_copied = 0;



}

void ResourceScene::GetBufferSize(uint & buffer_size)
{
	Resource::GetBufferSize(buffer_size);
}
